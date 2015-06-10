bool auth_user(String rfid)
{
    Serial.print("user auth for ");
    Serial.println(rfid);
    Process p;
    p.begin(query_cmd);
    p.addParameter("--check-user");
    p.addParameter("--rfid");
    p.addParameter(rfid);
    p.run();

    //command failed
    if(p.exitValue() != 0)
        return false;

    user.rfid = rfid;
    //start session timeout
    user.timeout = 0;

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
    {
        Serial.print("error");
        return 0;
    }

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

void timeout_user()
{
    user.name = "";
    user.rfid = "";
    for(int i = 0; i < MAX_TOOLS; i++)
        user.tools[i] = 0;
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

void stop_tool()
{
    tools[page_num].running = false;
    //turn off & log time
    radio_turn_off(tools[page_num].id);
    log_tool_time();
    tools[page_num].time = 0;
}

void start_tool()
{
    //log time & turn on
    Serial.print(F("starting tool id"));
    Serial.println(tools[page_num].id);
    tools[page_num].running = true;
    tools[page_num].time = millis()/1000;
    tools[page_num].current_user = user.name;
    radio_turn_on(tools[page_num].id);
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
