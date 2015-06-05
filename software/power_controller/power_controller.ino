//pin defs
#define ENC_A 3
#define ENC_B 2
#define BUT 4
#define BUT_LED 5

//either define RFID_SERIAL (serial based 125khz reader) or RFID_SPI (SPI based 13Mhz reader)
// #define RFID_SERIAL
#define RFID_SPI

#ifdef RFID_SERIAL
	#define RFID_NOT_ENABLE 6
	// Not all pins on the Leonardo support change interrupts,
	// so only the following can be used for RX:
	// 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
	#define RFID_RX 8
#elif defined RFID_SPI
	#define SS_PIN 8
	#define RST_PIN 6
#endif

#define HANDSHAKE 7 //do not use - reserved for future use?

#define RFID_TX RFID_RX //same as each other as we never send

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
#define UNKNOWN_USER_TIMEOUT 4000 //time before we scan again after bad rfid
#define MAX_TOOLS 10

//LCD
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);


#define S_USER_START 0
#define S_USER_IDLE 1
#define S_USER_READ_RFID 2
#define S_USER_CHECK_RFID 12
#define S_USER_VALID 3
#define S_USER_WAIT_CONTROL 4
#define S_USER_UNKNOWN 5
#define S_USER_TIMEOUT 6
#define S_USER_UPDATE_LCD 7
#define S_USER_BUTTON 8
#define S_USER_START_TOOL 9
#define S_USER_STOP_TOOL 10
#define S_USER_WAIT_RADIO 11

int fsm_state_user = S_USER_START;

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
} user;

//ui globals
int enc_clicks = 0; //encoder enc_clicks
bool button_pressed = false; //button's been pressed
int page_num = 0; //what page is showing on lcd
String rfid; //where rfid tags are read into
int num_tools = 0;
unsigned int msCounts = 0; //counter for states

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
        case S_USER_START:
            lcd_start();
            fsm_state_user = S_USER_IDLE;
            msCounts = 0;
            break;
        case S_USER_IDLE:
            //check rfid every 100ms
            if(msCounts >= 100)
                fsm_state_user = S_USER_READ_RFID;
            break;
        case S_USER_READ_RFID:
            rfid = read_rfid();
            if(rfid != "")
            {
                fsm_state_user = S_USER_CHECK_RFID; 
                Serial.println(free_memory());
            }
            else
            {
                msCounts = 0;
                fsm_state_user = S_USER_IDLE;
            } 
            break;
        case S_USER_CHECK_RFID:
            lcd_check_rfid();
            //check python command via bridge
            if(auth_user(rfid))
            {
                Serial.println(F("valid id"));
                msCounts = 0;
                page_num = 0;
                enc_clicks = 0;
                fsm_state_user = S_USER_VALID;
                lcd_valid_user();
            }
            else
            {
                fsm_state_user = S_USER_UNKNOWN;
                msCounts = 0;
                lcd_invalid_user();
            }
            break;
        case S_USER_VALID:
            if(msCounts >= LCD_TIMEOUT)
                fsm_state_user = S_USER_UPDATE_LCD;
            break;

        case S_USER_UNKNOWN:
            if(msCounts >= UNKNOWN_USER_TIMEOUT)
                fsm_state_user = S_USER_START;
            break;

        case S_USER_WAIT_CONTROL:
            if(check_controls())
                fsm_state_user = S_USER_UPDATE_LCD;
            //if user doesn't do anything for long enough, time out 

            if(msCounts >= SESSION_TIMEOUT)
            {
                fsm_state_user = S_USER_TIMEOUT;
                timeout_user();
                msCounts = 0;
                lcd_session_timeout();
            }
            break;

        case S_USER_UPDATE_LCD:
                //tool out of use
                if(tools[page_num].operational == false)
                {
                    lcd_tool_offline();
                }
                //is inducted?
                else if(! is_inducted(tools[page_num].id))
                {
                    lcd_tool_noinduct();
                }
                //it's not running
                else if(not tools[page_num].running)
                {
                    if(button_pressed)
                    {
                        fsm_state_user = S_USER_START_TOOL;
                        button_pressed = false;
                        break;
                    }
                    lcd_tool_start();
                }
                //it's running and we're the user
                else if(tools[page_num].current_user == user.name)
                {
                    if(button_pressed)
                    {
                        fsm_state_user = S_USER_STOP_TOOL;
                        button_pressed = false;
                        break;
                    }
                    lcd_tool_stop();
                }
                //it's running and we're not the user
                else
                {
                    lcd_tool_in_use();
                }
                msCounts = 0;
                fsm_state_user = S_USER_WAIT_CONTROL;
                break;
            
        case S_USER_START_TOOL:
                msCounts = 0;
                lcd_wait_radio();
                start_tool();
                fsm_state_user = S_USER_WAIT_RADIO;
                break;

        case S_USER_STOP_TOOL:
                msCounts = 0;
                lcd_wait_radio();
                stop_tool();
                fsm_state_user = S_USER_WAIT_RADIO;
                break;

        case S_USER_WAIT_RADIO:
            if(msCounts >= LCD_TIMEOUT)
                fsm_state_user = S_USER_UPDATE_LCD;
            break;
            
        case S_USER_TIMEOUT:
            if(msCounts >= LCD_TIMEOUT)
                 fsm_state_user = S_USER_START;
            break;
            
    }
    //Time keeping:
  delay(1);              // wait for a millisecond
  if(msCounts < 0xFFFF)  // Don't let the msCounts overflow
  {
    msCounts++;
  }
}
        

bool check_controls()
{
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
            //set button_pressed to false so don't accidentally press 
            //when a page changes
            button_pressed = false;
            return true;
        }
        if(digitalRead(BUT) == LOW)
        {
            button_pressed = true;
            //debounce
            while(digitalRead(BUT) == LOW)
                ;;
            return true;
        }

        return false;
}

void stop_tool()
{
    tools[page_num].running = false;
    //turn off & log time
    radio_turn_off(tools[page_num].id);
    log_tool_time();
}

void start_tool()
{
    //log time & turn on
    tools[page_num].running = true;
    tools[page_num].time = millis()/1000;
    tools[page_num].current_user = user.name;
    radio_turn_on(tools[page_num].id);
}
