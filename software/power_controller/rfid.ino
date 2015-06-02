#include <SoftwareSerial.h>
#define RFID_ID_LENGTH 10 + 2
SoftwareSerial rfid_serial(RFID_RX, RFID_TX); // RX, TX
String query_cmd = "/root/python_bridge/fetch.py";

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
        while(rfid_serial.available())
        {
            char c = rfid_serial.read();
            rfid.concat(c);
        }
        //trim newline
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
    p.begin(query_cmd);
    p.addParameter("--check-user");
    p.addParameter("--rfid");
    p.addParameter(rfid);
    p.run();

    if(p.exitValue() != 0)
        return false;

    user.rfid = rfid;

    if(p.available())
    {
        user.name = p.readStringUntil(',');
        Serial.print("name:"); Serial.println(user.name);
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
    Serial.println("fetch tools");
    Process p;
    p.begin(query_cmd);
    p.addParameter("--list-tools");
    p.run();

    if(p.exitValue() != 0)
        Serial.print("error");

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

            Serial.print("tool id:"); Serial.print(tools[num_tools].id);
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
    Serial.println(F("logging time"));
    Process p;
    p.begin(query_cmd);
    p.addParameter("--log-tool");
    p.addParameter(tools[page_num].name);
    p.addParameter("--rfid");
    p.addParameter(user.rfid);
    p.addParameter("--time");
    p.addParameter(lcd_format_time(tools[page_num].time));
    p.run();
    Serial.println("run");
    Serial.println(free_memory());
    //async so we don't have to wait - didn't work because p goes out of scope
//    p.runAsynchronously();
}

extern unsigned int __data_start;
extern unsigned int __data_end;
extern unsigned int __bss_start;
extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;

int free_memory() {
    int free_memory;
  
    if((int)__brkval == 0)
        free_memory = ((int)&free_memory) - ((int)&__bss_end);
    else
        free_memory = ((int)&free_memory) - ((int)__brkval);
    return free_memory;
};
