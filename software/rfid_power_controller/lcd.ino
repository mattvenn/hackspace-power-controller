
#define LCD_WIDTH 20
void setup_lcd() {
  // set up the LCD's number of columns and rows: 
  lcd.begin(LCD_WIDTH, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
}

void lcd_start()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("scan RFID to start");
  lcd.setCursor(0, 1);
  lcd.print("goto: ven.nz/hkspcpc");
}

void lcd_valid_user(int user_id)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("user: ");
  lcd.print(users[user_id].user_name);
  lcd.setCursor(0, 1);
  lcd.print("scroll for tools");
}

void lcd_invalid_user()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("unrecognised RFID");
    lcd.setCursor(0, 1);
    lcd.print("goto: ven.nz/hkspcpc");
}

void lcd_show_tool(int tool_id, int user_id)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(tool_id);
    lcd.print(": ");
    lcd.print(tools[tool_id].tool_name);
    lcd.setCursor(LCD_WIDTH - 8, 0);

    if(tools[tool_id].operational == false)
    {
        lcd.print("offline");
        lcd.setCursor(0, 1);
        lcd.print("goto: ven.nz/hkspcpc");
        //button not available, so turn off light
        digitalWrite(BUT_LED, LOW);
    }
    else if(is_inducted(user_id, tool_id))
    {
        lcd.print("inducted");
        if(tools[tool_id].running)
        {
            if(tools[tool_id].current_user == user_id)
            {
                digitalWrite(BUT_LED, HIGH);
                lcd.setCursor(0, 1);
                lcd.print("select to stop");
            }
            else
            {
                int current_user = tools[tool_id].current_user;
                digitalWrite(BUT_LED, LOW);
                lcd.setCursor(0, 1);
                lcd.print(users[current_user].user_name);
                lcd.setCursor(LCD_WIDTH - 8, 1);
                lcd.print(lcd_format_time(tools[tool_id].time));
            }
        }
        else
        {
            digitalWrite(BUT_LED, HIGH);
            lcd.setCursor(0, 1);
            lcd.print("select to start");
        } 
    }
    else
    {
        digitalWrite(BUT_LED, LOW);
        lcd.print("noinduct");
        lcd.setCursor(0, 1);
        lcd.print("goto: ven.nz/hkspcpc");
    }

}

int lcd_format_time(int seconds)
{
    return millis()/1000 - seconds; 
}
