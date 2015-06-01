// Not all pins on the Leonardo support change interrupts,
// so only the following can be used for RX:
// 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
#include <SoftwareSerial.h>
SoftwareSerial mySerial(RFID_RX, RFID_TX); // RX, TX

void setup_rfid()
{
  mySerial.begin(2400);
  pinMode(RFID_NOT_ENABLE, OUTPUT);
  //turn on rfid
  digitalWrite(RFID_NOT_ENABLE, LOW);
}
#define RFID_ID_LENGTH 10 + 2
String check_rfid()
{
    Serial.println("check rfid");
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

/*
int get_user_id(String rfid)
{
    Process p;
    String command = "/root/spreadsheet/fetch.py --check-rfid " + rfid;
    Serial.println(command);
    p.runShellCommand(command);

    if(p.exitValue() != 0)
        return -1;

    int id = 0;
    String name = "";

    if(p.available())
        id = p.parseInt();

    while(p.available()) 
    {
        char c = p.read();
        if(c!='\n')
            name += c;
    }
    Serial.println(name);
    Serial.println(id);
    return id;
}
*/
int get_user_id(String rfid)
{
    for(int i = 0; i < num_users; i ++)
    {
        if(users[i].rfid == rfid)
            return i;
    }
    return -1;
}

bool is_inducted(int user_id, int tool_id)
{
    for(int i = 0; i < num_users; i ++)
    {
        if(tools[tool_id].users[i] == user_id)
            return true;
    }
    return false;
}
