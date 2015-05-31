//pin defs
#define ENCA_a 6
#define ENCA_b 7
#define RFID_RX 4
#define RFID_TX RFID_RX //same as each other as we never send
#define BUT 2
#define BUT_LED 5
#define RADIO_TX 3
#include <LiquidCrystal.h>

//UI defs
#define LCD_TIMEOUT 1000 //time we spend before changing screen
#define SESSION_TIMEOUT 5000 //time before session times out

// initialize the library with the numbers of the interface pins
//rs enable d4,5,6,7
LiquidCrystal lcd(8,9,10,11,12,13);

#define num_tools 4
#define num_users 2

#define S_USER_START 0
#define S_USER_IDLE 1
#define S_USER_CHECK_RFID 2
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
    String tool_name;
    bool running = false;
    int current_user = 0;
    bool operational = true;
    int time = 0;
    int users[num_users];
};

struct user
{
    String user_name;
    String rfid;
};

struct tool tools[num_tools];
struct user users[num_users];

int enc_clicks = 0; //encoder enc_clicks
bool button_pressed = false; //button is pressed
int tool_id = 0; //what page is showing on lcd
int user_id = -1;
double session_time;

void setup()
{
    Serial.begin(9600); 
    Serial.println("started");
    setup_lcd();
    setup_radio();
    setup_rfid();

    //button setup
    pinMode(BUT, INPUT);
    digitalWrite(BUT, HIGH);
    pinMode(BUT_LED, OUTPUT);
    digitalWrite(BUT_LED, LOW);

    //fetch users & tools
    setup_tools();
    setup_users();

    lcd_start();
}
unsigned int msCounts = 0;

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
         if(msCounts >= 100)
                fsm_state_user = S_USER_CHECK_RFID;
            break;
        case S_USER_CHECK_RFID:
        {
            String rfid = check_rfid();
            if(rfid != "")
            {
                user_id = get_user_id(rfid);
                //valid user id?
                if(user_id >=0)
                {
                    Serial.println("valid id");
                    msCounts = 0;
                    fsm_state_user = S_USER_VALID;
                    lcd_valid_user();
                }
                else
                {
                    fsm_state_user = S_USER_UNKNOWN;
                    msCounts = 0;
                    lcd_invalid_user();
                }
            }
            else
            {
                msCounts = 0;
                fsm_state_user = S_USER_IDLE;
            } 
            break;
        }
        case S_USER_VALID:
            if(msCounts >= LCD_TIMEOUT)
                fsm_state_user = S_USER_UPDATE_LCD;
            break;
        case S_USER_UNKNOWN:
             if(msCounts >= LCD_TIMEOUT)
                fsm_state_user = S_USER_START;
            break;
        case S_USER_WAIT_CONTROL:
            if(check_controls())
                fsm_state_user = S_USER_UPDATE_LCD;
             if(msCounts >= SESSION_TIMEOUT)
             {
                fsm_state_user = S_USER_TIMEOUT;
                msCounts = 0;
                lcd_session_timeout();
            }
            break;
        case S_USER_UPDATE_LCD:
                //tool out of use
                if(tools[tool_id].operational == false)
                {
                    digitalWrite(BUT_LED, LOW);
                    lcd_tool_offline();
                }
                //is inducted?
                else if(! is_inducted(user_id, tool_id))
                {
                    digitalWrite(BUT_LED, LOW);
                    lcd_tool_noinduct();
                }
                //it's not running
                else if(not tools[tool_id].running)
                {
                    digitalWrite(BUT_LED, HIGH);
                    if(button_pressed)
                    {
                        fsm_state_user = S_USER_START_TOOL;
                        button_pressed = false;
                        break;
                    }
                    lcd_tool_start();
                }
                //it's running and we're the user
                else if(tools[tool_id].current_user == user_id)
                {
                    digitalWrite(BUT_LED, HIGH);
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
                    digitalWrite(BUT_LED, LOW);
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
            if(msCounts >= SESSION_TIMEOUT)
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
        if(tool_id != enc_clicks)
        {
            session_refresh();
            tool_id = enc_clicks;
            Serial.print("showing tool id: ");
            Serial.println(enc_clicks);
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
    tools[tool_id].running = false;
    //log time & turn off
    radio_turn_off(tool_id);
    Serial.println("logging tool off time");
}

void start_tool()
{
    //log time & turn on
    tools[tool_id].running = true;
    tools[tool_id].time = millis()/1000;
    tools[tool_id].current_user = user_id;
    Serial.println("logging tool on time");
    radio_turn_on(tool_id);
}
