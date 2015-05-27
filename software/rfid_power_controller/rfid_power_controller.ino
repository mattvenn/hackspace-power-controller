#include <ByteBuffer.h>
#include <ooPinChangeInt.h> // necessary otherwise we get undefined reference errors.
#define DEBUG
#ifdef DEBUG
ByteBuffer printBuffer(200);
#endif
#include <AdaEncoder.h>

#define ENCA_a 6
#define ENCA_b 7


AdaEncoder encoderA = AdaEncoder('a', ENCA_a, ENCA_b);

int8_t clicks=0;
char id=0;

#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
//rs enable d4,5,6,7
LiquidCrystal lcd(8,9,10,11,12,13);

void setup()
{
  Serial.begin(9600); 
  setup_lcd();
  setup_radio();
}

int count = 0;

void loop() 
{
 // char outChar;
 // while ((outChar=(char)printBuffer.get()) != 0) Serial.print(outChar);
  check_encoder();
  
    // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis()/1000);
  lcd.setCursor(10,2);
  lcd.print(count);
  if( count == 10 )
  {
    turn_on();
  }
  if(count == 11 )
  {
      turn_off();
  }

}

