#ifdef RFID_SERIAL
	#include <SoftwareSerial.h>
	#define RFID_ID_LENGTH 10 + 2
	SoftwareSerial rfid_serial(RFID_RX, RFID_TX); // RX, TX
#elif defined RFID_SPI
	#include <SPI.h>
	#include <MFRC522.h>
	MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance
#endif

String query_cmd = "/root/query.py";

void setup_rfid()
{
#ifdef RFID_SERIAL
    rfid_serial.begin(2400);
    rfid_serial.setTimeout(10);
    //turn on rfid
    pinMode(RFID_NOT_ENABLE, OUTPUT);
    digitalWrite(RFID_NOT_ENABLE, LOW);
#elif defined RFID_SPI
    SPI.begin();			// Init SPI bus
    mfrc522.PCD_Init();		// Init MFRC522
#endif
}

String read_rfid()
{
    String rfid = "";
#ifdef RFID_SERIAL
    if(rfid_serial.available() == RFID_ID_LENGTH)
    {
        for(int i = 0; i < RFID_ID_LENGTH; i++)
        {
            rfid.concat((char)rfid_serial.read());
        }
        //trim newline
        rfid.trim();
        Serial.print("got RFID: "); Serial.println(rfid);
    }
    else if(rfid_serial.available() > RFID_ID_LENGTH)
        rfid_serial.flush();
#elif defined RFID_SPI
	// Look for new cards
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return rfid;
	}
	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return rfid;
	}
	
        mfrc522.PICC_HaltA();
	for (byte i = 0; i < mfrc522.uid.size; i++) 
	{
		if(mfrc522.uid.uidByte[i] < 0x10)
		rfid.concat("0");
		rfid.concat(String(mfrc522.uid.uidByte[i], HEX));
	} 
        Serial.print("got RFID: "); Serial.println(rfid);
#endif
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
    Serial.println("starting user auth");
    Process p;
    p.begin(query_cmd);
    p.addParameter("--check-user");
    p.addParameter("--rfid");
    p.addParameter(rfid);
    Serial.println("about to run");
    p.run();
    Serial.println("finished");

    //command failed
    if(p.exitValue() != 0)
        return false;

    user.rfid = rfid;

    p.setTimeout(1000);
    Serial.println("is available?");
    if(p.available())
    {
        Serial.println("reading till,");
        user.name = p.readStringUntil(',');
        Serial.println("read");
        Serial.print("name:"); Serial.println(user.name);
        int i = 0;
        Serial.println("reading tools");
        while(p.available() > 1) //more than one means we skip the \n
        {
            Serial.println("about to parseint");
            user.tools[i] = p.parseInt();
            Serial.println("done");
            Serial.print("tool:"); Serial.println(user.tools[i]);
            i++;
        }
    }

    return(user.name != "");
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
