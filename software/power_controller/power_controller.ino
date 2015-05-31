//pin defs
#define ENCA_a 6
#define ENCA_b 7
#define RFID_RX 4
#define RFID_TX RFID_RX //same as each other as we never send
#define BUT 2
#define BUT_LED 5
#define RADIO_TX 3
#include <LiquidCrystal.h>
#include <Process.h>

//UI defs
#define LCD_TIMEOUT 500 //time we spend before changing screen
#define SESSION_TIMEOUT 5000 //time before session times out

// initialize the library with the numbers of the interface pins
//rs enable d4,5,6,7
LiquidCrystal lcd(8,9,10,11,12,13);

#define num_tools 4
#define num_users 2

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

    Bridge.begin();	// Initialize the Bridge

    // Wait until a Serial Monitor is connected.
    while (!Serial);

    //get_user_id("04184A4FC6"); //correct
    Serial.println(get_user_id("04184A4FC5"));
}

void loop() 
{
    //if no session
    if(!session_running(user_id))
    {
        //check for rfid
        String rfid = check_rfid();
        if(rfid != "")
        {
            user_id = get_user_id(rfid);
            //valid user id?
            if(user_id >=0)
            {
                Serial.println("valid id");
                //say hi
                lcd_valid_user(user_id);
                delay(LCD_TIMEOUT);
                session_start();
                lcd_show_tool(tool_id, user_id);
            }
            else
            {
                lcd_invalid_user();
                delay(LCD_TIMEOUT);
                lcd_start();
            }
        }
    }

    //timeout a validated user
    if(user_id >= 0 && session_timed_out())
        session_end();
       
    //we have a valid user, so let them use the encoder to select tools
    if(user_id >= 0)
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
            lcd_show_tool(tool_id, user_id);
        }

        //we only care about button presses if a valid user & inducted
        if(is_inducted(user_id, tool_id) && tools[tool_id].operational)
        {
            if(digitalRead(BUT) == LOW)
            {
                Serial.println("button pressed");
                session_refresh();
                //running
                if(tools[tool_id].running)
                {
                    //only turn off if we're the current user
                    if(tools[tool_id].current_user == user_id)
                    {
                        tools[tool_id].running = false;
                        //log time & turn off
                        radio_turn_off(tool_id);
                        Serial.println("logging tool off time");
                    }
                }
                else
                {
                    //log time & turn on
                    tools[tool_id].running = true;
                    tools[tool_id].time = millis()/1000;
                    tools[tool_id].current_user = user_id;
                    Serial.println("logging tool on time");
                    radio_turn_on(tool_id);
                }

                //update lcd
                lcd_show_tool(tool_id, user_id);
            }
        }
    }
}

