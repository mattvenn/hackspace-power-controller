// Not all pins on the Leonardo support change interrupts,
// so only the following can be used for RX:
// 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
#include <SoftwareSerial.h>
SoftwareSerial mySerial(RFID_RX, RFID_TX); // RX, TX

void setup_rfid()
{
  mySerial.begin(2400);
}
#define RFID_ID_LENGTH 10 + 2
String check_rfid()
{
  String id = "";
  int i = 0;
  if(mySerial.available() == RFID_ID_LENGTH)
  {
    Serial.println("got an ID");
    while(i++ < RFID_ID_LENGTH)
    {
        char c = mySerial.read();
        if(i == 1 || i == RFID_ID_LENGTH)
            continue;
        id += c;
    }
    Serial.println(id);
  }
  //flush if somehow there's too much data eg multiple reads or corrupted data
  else if(mySerial.available() > RFID_ID_LENGTH)
  {
    mySerial.flush();
  }
  return id;
}
