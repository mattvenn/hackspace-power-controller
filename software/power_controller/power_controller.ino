//pin defs
#define ENC_A 3
#define ENC_B 2
#define BUT 4
#define BUT_LED 5

//either define RFID_SERIAL (serial based 125khz reader) or RFID_SPI (RC522 SPI based 13Mhz reader)
#define RFID_SERIAL
//#define RFID_SPI

#ifdef RFID_SERIAL
	#define RFID_NOT_ENABLE 6
	// Not all pins on the Leonardo support change interrupts,
	// so only the following can be used for RX:
	// 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
	#define RFID_RX 8
    #define RFID_TX RFID_RX //same as each other as we never send
#elif defined RFID_SPI
	#define SS_PIN 8
	#define RST_PIN 6
#endif

#define HANDSHAKE 7 //do not use - reserved for future use?


#define LCD_D6 9
#define LCD_D7 10
#define LCD_D4 11
#define LCD_D5 12
#define LCD_EN 13
#define LCD_RS A0

#define GPIO1 A3 //unused
#define GPIO2 A2
#define GPIO3 A1

#define RADIO_TX A4
#define BUZZ A5

#include <LiquidCrystal.h>
#include <Process.h>

//UI defs
#define LCD_TIMEOUT 1000 //time we spend before changing screen
#define SESSION_TIMEOUT 5000 //time before session times out
#define RFID_INVALID_TIMEOUT 4000 //time before we scan again after bad rfid
#define MAX_TOOLS 3 //you must have enough radio codes defined in radio.ino
#define TOOL_UPDATE_PERIOD 18000000 //ms in half an hour

//LCD
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

//FSM states
#define S_START 0
#define S_WAIT_RFID 1
#define S_READ_RFID 2
#define S_CHECK_RFID 3
#define S_RFID_VALID 4
#define S_RFID_INVALID 5
#define S_WAIT_CONTROL 6
#define S_LCD_SHOW_TOOL 7
#define S_TIMEOUT 8
#define S_START_TOOL 9
#define S_STOP_TOOL 10
#define S_WAIT_RADIO 11

//array of tool data
struct tool
{
    String name;
    bool running = false;
    String current_user = "";
    bool operational = true;
    int time = 0;
    int id = 0; //0 is an invalid id
} tools[MAX_TOOLS];

//authenticated user's info
struct user
{
    String name = "";
    String rfid = "";
    int tools[MAX_TOOLS];
    int timeout = 0;
} user;

//ui globals
int fsm_state_user = S_START;
int enc_clicks = 0; //encoder enc_clicks
int page_num = 0; //what page is showing on lcd
String rfid; //where rfid tags are read into
int num_tools = 0;
unsigned int state_timer = 0; //counter for states
String query_cmd = "/root/query.py"; //what we use to query users/log tool time
unsigned long int last_updated_tools = 0;


void setup()
{
    Serial.begin(9600); 
    Serial.println(F("started"));

    //peripherals
    setup_lcd();
    setup_radio();
    setup_rfid();

    //button setup
    pinMode(BUT, INPUT);
    digitalWrite(BUT, HIGH);
    pinMode(BUT_LED, OUTPUT);
    digitalWrite(BUT_LED, LOW);

	//initialize the Bridge, takes a few seconds
    lcd_boot_screen();
    Bridge.begin();

    //fetch users & tools
    num_tools = get_tools();
}

void loop() 
{
    switch(fsm_state_user)
    {
        case S_START:
            lcd_start();
            start_rfid();
            fsm_state_user = S_WAIT_RFID;
            state_timer = 0;
            break;
        
        case S_WAIT_RFID:
            if(state_timer >= 50)
                fsm_state_user = S_READ_RFID;
            break;

        case S_READ_RFID:
            rfid = read_rfid();
            if(rfid != "")
                fsm_state_user = S_CHECK_RFID; 
            else
            {
                state_timer = 0;
                fsm_state_user = S_WAIT_RFID;
            } 
            break;

        case S_CHECK_RFID:
            lcd_check_rfid();
            //check python command via bridge
            if(auth_user(rfid))
            {
                Serial.println(F("valid id"));
                state_timer = 0;
                page_num = 0;
                enc_clicks = 0;
                fsm_state_user = S_RFID_VALID;
                lcd_valid_user();
            }
            else
            {
                fsm_state_user = S_RFID_INVALID;
                state_timer = 0;
                lcd_invalid_user();
            }
            break;

        case S_RFID_VALID:
            if(state_timer >= LCD_TIMEOUT)
                fsm_state_user = S_LCD_SHOW_TOOL;
            break;

        case S_RFID_INVALID:
            if(state_timer >= RFID_INVALID_TIMEOUT)
                fsm_state_user = S_START;
            break;

        case S_WAIT_CONTROL:
        {
            //update lcd as soon as encoder changes
            if(encoder_changed())
            {
                fsm_state_user = S_LCD_SHOW_TOOL;
                digitalWrite(BUT_LED, LOW);
                state_timer = 0;
                user.timeout = 0;
            }
            //button press logic, only allow inducted users to do things
            if(button_pressed())
            {
                state_timer = 0;
                user.timeout = 0;

                //only if tool operational
                if(tools[page_num].operational)
                    if(is_inducted(tools[page_num].id))
                        if(tools[page_num].running)
                        {
                            if(tools[page_num].current_user == user.name)
                                fsm_state_user = S_STOP_TOOL;
                        }
                        else
                            fsm_state_user = S_START_TOOL;
            }            
                            
            //update lcd regularly
            if(state_timer >= LCD_TIMEOUT)
                fsm_state_user = S_LCD_SHOW_TOOL;

            //if user doesn't do anything for long enough, time out 
            if(user.timeout >= SESSION_TIMEOUT)
            {
                Serial.println(F("auth timeout"));
                fsm_state_user = S_TIMEOUT;
                digitalWrite(BUT_LED, LOW);
                timeout_user();
                state_timer = 0;
                lcd_session_timeout();
            }
            break;
        }
        case S_LCD_SHOW_TOOL:
            lcd_show_tool_page();
            fsm_state_user = S_WAIT_CONTROL;
            state_timer = 0;
            break;

        case S_START_TOOL:
            digitalWrite(BUT_LED, LOW);
            state_timer = 0;
            lcd_wait_radio_on();
            start_tool();
            fsm_state_user = S_WAIT_RADIO;
            break;

        case S_STOP_TOOL:
            digitalWrite(BUT_LED, LOW);
            state_timer = 0;
            lcd_wait_radio_off();
            stop_tool();
            fsm_state_user = S_WAIT_RADIO;
            break;

        case S_WAIT_RADIO:
            if(state_timer >= LCD_TIMEOUT)
                fsm_state_user = S_LCD_SHOW_TOOL;
            break;
            
        case S_TIMEOUT:
            if(state_timer >= LCD_TIMEOUT)
                 fsm_state_user = S_START;
            break;
            
    }

    //FSM time keeping
    delay(1);
    if(state_timer < 0xFFFF)  // Don't let the state_timer overflow
    {
        state_timer++;
        user.timeout++;
    }

    //Update tool table regularly
    if(millis() > last_updated_tools + TOOL_UPDATE_PERIOD)
    {
        //only update if no one using tools right now
        if(can_update_tools())
        {
            Serial.println(F("updating tools"));
            num_tools = get_tools();
            last_updated_tools = millis();
        }
    }
}
        
bool encoder_changed()
{
    //check encoder - could use interrupts instead of polling
    check_encoder();

    //limit to num of tools available
    if(enc_clicks < 0)
        enc_clicks = 0;
    if(enc_clicks > num_tools - 1)
        enc_clicks = num_tools - 1;

    //different to last time? update display
    if(page_num != enc_clicks)
    {
        page_num = enc_clicks;
        return true;
    }
    else
        return false;
}

bool button_pressed()
{
    //check button
    if(digitalRead(BUT) == LOW)
    {
        //debounce
        while(digitalRead(BUT) == LOW);
        return true;
    }
    else
        return false;
}

//memory tracking
/*
extern unsigned int __data_start;
extern unsigned int __data_end;
extern unsigned int __bss_start;
extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;

int free_memory() {
    int free_memory;
  
    if((int)__brkval == 0)
        free_memory = ((int)&free_memory) - ((int)&__bss_end);
    else
        free_memory = ((int)&free_memory) - ((int)__brkval);
    return free_memory;
};
*/
