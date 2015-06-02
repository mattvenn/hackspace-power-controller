#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup_radio() {

  // Transmitter is connected to Arduino Pin #10  
  pinMode(RADIO_TX,OUTPUT);
  mySwitch.enableTransmit(RADIO_TX);
 
  // Optional set protocol (default is 1, will work for most outlets)
   mySwitch.setProtocol(2);

  // Optional set pulse length.
   mySwitch.setPulseLength(500);
  
  // Optional set number of transmission repetitions.
   mySwitch.setRepeatTransmit(20);  
}

void radio_turn_on(int tool_id)
{
    switch(tool_id)
    {
        case 1:
            mySwitch.send("101001000110001010101100"); //1
            break;
        case 2:
            mySwitch.send("101011011010011000000101"); //2
            break;
        case 3:
            mySwitch.send("101000001100100101111110"); //3
            break;
        default:
            Serial.println(F("no radio id"));
    }
}
void radio_turn_off(int tool_id)
{
    switch(tool_id)
    {
        case 1:
            mySwitch.send("101001110111110010111100"); //1
            break;
        case 2:
            mySwitch.send("101000010101101001000101"); //2
            break;
        case 3:
            mySwitch.send("101010111011000001011110"); //3
            break;
        default:
            Serial.println(F("no radio id"));
    }
}
