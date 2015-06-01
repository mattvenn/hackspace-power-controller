
#define LCD_WIDTH 20

void setup_lcd()
{
    // set up the LCD's number of columns and rows: 
    lcd.begin(LCD_WIDTH, 2);
    lcd_boot();
}

void lcd_boot()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("power controller");
    lcd.setCursor(0, 1);
    lcd.print("waiting for Yun...");
}

void lcd_start()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("scan RFID to start");
    lcd.setCursor(0, 1);
    lcd.print("ven.nz/hkspcpc");
    digitalWrite(BUT_LED, LOW);
}

void lcd_session_timeout()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("access timed out");
    digitalWrite(BUT_LED, LOW);
}

void lcd_valid_user()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("user: ");
    lcd.print(user_name);
    lcd.setCursor(0, 1);
    lcd.print("scroll for tools");
    digitalWrite(BUT_LED, LOW);
}

void lcd_invalid_user()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("unrecognised RFID");
    digitalWrite(BUT_LED, LOW);
}

void lcd_check_rfid()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("checking RFID");
    digitalWrite(BUT_LED, LOW);
}

void lcd_tool_offline()
{
    lcd_tool_id();
    lcd.print("offline");
    //button not available, so turn off light
    digitalWrite(BUT_LED, LOW);
}

void lcd_wait_radio()
{
    lcd_tool_id();
    lcd.print("inducted");
    lcd.setCursor(0, 1);
    lcd.print("sending signal");
    digitalWrite(BUT_LED, LOW);
}

void lcd_tool_noinduct()
{
    lcd_tool_id();
    lcd.print("noinduct");
    digitalWrite(BUT_LED, LOW);
}

void lcd_tool_in_use()
{
    lcd_tool_id();
    lcd.print("inducted");
    lcd.setCursor(0, 1);
    lcd.print(tools[tool_id].current_user);
    lcd.setCursor(LCD_WIDTH - 8, 1);
    lcd.print(lcd_format_time(tools[tool_id].time));
    digitalWrite(BUT_LED, LOW);
}

void lcd_tool_id()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(tool_id);
    lcd.print(": ");
    lcd.print(tools[tool_id].tool_name);
    lcd.setCursor(LCD_WIDTH - 8, 0);
}

void lcd_tool_stop()
{
    lcd_tool_id();
    lcd.print("inducted");
    lcd.setCursor(0, 1);
    lcd.print("select to stop");
    digitalWrite(BUT_LED, HIGH);
}

void lcd_tool_start()
{
    lcd_tool_id();
    lcd.print("inducted");
    lcd.setCursor(0, 1);
    lcd.print("select to start");
    digitalWrite(BUT_LED, HIGH);
} 

char * lcd_format_time(unsigned long t)
{
    static char str[8];
    t = millis() / 1000 - t;
    int h = t / 3600;
    t = t % 3600;
    int m = t / 60;
    int s = t % 60;
    sprintf(str, "%02d:%02d:%02d", h, m, s);
    return str;
}
