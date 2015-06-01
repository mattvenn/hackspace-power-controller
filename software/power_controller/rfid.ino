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
    String id = "";
    int i = 0;
    if(mySerial.available() == RFID_ID_LENGTH)
    {
        while(i++ < RFID_ID_LENGTH)
        {
            char c = mySerial.read();
            if(i == 1 || i == RFID_ID_LENGTH)
                continue;
            id += c;
        }
        Serial.print("got RFID: "); Serial.println(id);
    }
    //flush if somehow there's too much data eg multiple reads or corrupted data
    else if(mySerial.available() > RFID_ID_LENGTH)
    {
        mySerial.flush();
    }
    return id;
}

int get_user_id(String rfid)
{
    Process p;
    String command = "/root/spreadsheet/fetch.py --check-user --rfid " + rfid;
    Serial.println(command);
    p.runShellCommand(command);

    if(p.exitValue() != 0)
        return -1;

    int id = 0;
    String name = "";

    if(p.available())
        id = p.parseInt();
    
    //strip space
    p.read();

    //read the name
    while(p.available()) 
    {
        char c = p.read();
        if(c!='\n')
            name += c;
    }
    Serial.println(name);
    Serial.println(id);
    user_name = name;
    return id;
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

void log_tool_time()
{
    Serial.println("logging tool off time");
    Process p;
    p.begin("/root/spreadsheet/fetch.py");
    p.addParameter("--log-tool");
    p.addParameter(tools[tool_id].tool_name);
    p.addParameter("--rfid");
    p.addParameter(rfid);
    p.addParameter("--time");
    p.addParameter(lcd_format_time(tools[tool_id].time));
    p.run();
}
