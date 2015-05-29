//pin defs
#define ENCA_a 6
#define ENCA_b 7
#define RFID_RX 4
#define RFID_TX RFID_RX //same as each other as we never send

#define USE_ENCODER
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
//rs enable d4,5,6,7
LiquidCrystal lcd(8,9,10,11,12,13);


void setup()
{
  Serial.begin(9600); 
  Serial.println("started");
  setup_lcd();
  setup_radio();
  setup_rfid();
}

int count = 0;

String valid = "04184A4FC6";
//04184A4147

typedef struct
{
    unsigned int fuelcellStatus;
    float fuelcellAmbientT;
    float fuelcellStackV;
} Message;
Message message;

void loop() 
{
  #ifdef USE_ENCODER
  check_encoder();
  #endif

  String id = check_rfid();
  if(id == valid)
  {
    Serial.println("valid id");
  }
    
  
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

