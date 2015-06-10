#define LCD_WIDTH 20

void setup_lcd()
{
    // set up the LCD's number of columns and rows: 
    lcd.begin(LCD_WIDTH, 2);
}

void lcd_boot_screen()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("power controller"));
    lcd.setCursor(0, 1);
    lcd.print(F("waiting for Yun..."));
}

void lcd_start()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("scan RFID to start"));
    lcd.setCursor(0, 1);
    lcd.print(F("ven.nz/hkspcpc"));
}

void lcd_session_timeout()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("access timed out"));
}

void lcd_valid_user()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("user: "));
    lcd.print(user.name);
    lcd.setCursor(0, 1);
    lcd.print(F("scroll for tools"));
}

void lcd_invalid_user()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("unrecognised RFID"));
}

void lcd_check_rfid()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("checking RFID"));
}

void lcd_wait_radio_on()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("transmitting on"));
}

void lcd_wait_radio_off()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("transmitting off"));
    lcd.setCursor(0, 1);
    lcd.print(F("logging usage"));
}

//format time nicely
char * lcd_format_time(unsigned long t)
{
    static char str[9];
    t = millis() / 1000 - t;
    int h = t / 3600;
    t = t % 3600;
    int m = t / 60;
    int s = t % 60;
    sprintf(str, "%02d:%02d:%02d\0", h, m, s);
    return str;
}

//main routine that shows all the info about a tool
void lcd_show_tool_page()
{
    lcd.clear();
    
    //1st line
    lcd.setCursor(0, 0);

    //start with tool id
    lcd.print(page_num + 1);
    lcd.print(": ");
    lcd.print(tools[page_num].name);
    lcd.setCursor(LCD_WIDTH - 8, 0);

    //tool out of use
    if(tools[page_num].operational == false)
    {
        lcd.print(F("offline"));
        return;
    }
    //now show if inducted or not
    else if(is_inducted(tools[page_num].id))
        lcd.print(F("inducted"));
    else
        lcd.print(F("noinduct"));

    //2nd line of lcd
    lcd.setCursor(0, 1);

    //if tool running
    if(tools[page_num].running)
    {
        //and we're the current user
        if(tools[page_num].current_user == user.name)
        {
            lcd.print(F("stop?"));
            digitalWrite(BUT_LED, HIGH);
        }
        else
            lcd.print(tools[page_num].current_user);

        //show run time
        lcd.setCursor(LCD_WIDTH - 8, 1);
        lcd.print(lcd_format_time(tools[page_num].time));
    }
    //if not running and we're inducted
    else if(is_inducted(tools[page_num].id))
    {
        lcd.print(F("start?"));
        digitalWrite(BUT_LED, HIGH);
    }

}
