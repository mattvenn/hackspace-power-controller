
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup_radio() {

  // Transmitter is connected to Arduino Pin #10  
 pinMode(3,OUTPUT);
  mySwitch.enableTransmit(3);
 
  // Optional set protocol (default is 1, will work for most outlets)
   mySwitch.setProtocol(2);

  // Optional set pulse length.
   mySwitch.setPulseLength(500);
  
  // Optional set number of transmission repetitions.
   mySwitch.setRepeatTransmit(6);  
}

void turn_on()
{

 // mySwitch.send("101001000110001010101100"); //1
    mySwitch.send("101011011010011000000101"); //2
//  mySwitch.send("101000001100100101111110"); //3

}
void turn_off()
{
 // mySwitch.send("101001110111110010111100"); //1
    mySwitch.send("101000010101101001000101"); //2
 // mySwitch.send("101010111011000001011110"); //3
}
