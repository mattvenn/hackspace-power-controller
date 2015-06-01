#include <SoftwareSerial.h>
#define RFID_ID_LENGTH 10 + 2
SoftwareSerial rfid_serial(RFID_RX, RFID_TX); // RX, TX

void setup_rfid()
{
    rfid_serial.begin(2400);
    rfid_serial.setTimeout(10);
    //turn on rfid
    pinMode(RFID_NOT_ENABLE, OUTPUT);
    digitalWrite(RFID_NOT_ENABLE, LOW);
}

String read_rfid()
{
    String rfid = "";
    if(rfid_serial.available() == RFID_ID_LENGTH)
    {
        //get rid of first header char
        rfid_serial.read();
        rfid = rfid_serial.readString();
        rfid.trim();
        Serial.print("got RFID: "); Serial.println(rfid);
    }
    else if(rfid_serial.available() > RFID_ID_LENGTH)
        rfid_serial.flush();
    return rfid;
}

void timeout_user()
{
    user.name = "";
    user.rfid = "";
    for(int i = 0; i < MAX_TOOLS; i++)
        user.tools[i] = 0;
}
bool auth_user(String rfid)
{
    Process p;
    String command = "/root/spreadsheet/fetch.py --check-user --rfid " + rfid;
    Serial.print(command);
    p.runShellCommand(command);

    if(p.exitValue() != 0)
        return false;

    user.rfid = rfid;

    if(p.available())
    {
        user.name = p.readStringUntil(',');
        Serial.print("got name: "); Serial.println(user.name);
        int i = 0;
        while(p.available() > 1) //more than one means we skip the \n
        {
            user.tools[i] = p.parseInt();
            Serial.print("tool:"); Serial.println(user.tools[i]);
            i++;
        }
    }
    return true;
}

int get_tools()
{
    Serial.println("fetching list of tools");
    Process p;
    String command = "/root/spreadsheet/fetch.py --list-tools";
    Serial.println(command);
    p.runShellCommand(command);

    if(p.exitValue() != 0)
        Serial.print("error fetching tools");

    int num_tools = 0;
    if(p.available())
    {
        while(p.available())
        {
            //name
            tools[num_tools].name = p.readStringUntil(',');
            //read the tool's id and whether it's working at the moment
            tools[num_tools].id = p.parseInt();
            tools[num_tools].operational = p.parseInt();
            //chomp the newline
            p.read();

            Serial.print("tool id: "); Serial.print(tools[num_tools].id);
            Serial.print(" name:"); Serial.println(tools[num_tools].name);
            num_tools++;
        }
    }
    return num_tools;
}

bool is_inducted(int tool_id)
{
    for(int i = 0; i < MAX_TOOLS; i ++)
    {
        if(user.tools[i] == tool_id)
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
    p.addParameter(tools[page_num].name);
    p.addParameter("--rfid");
    p.addParameter(user.rfid);
    p.addParameter("--time");
    p.addParameter(lcd_format_time(tools[page_num].time));
    p.run();
    //async so we don't have to wait - didn't work because p goes out of scope
//    p.runAsynchronously();
}
