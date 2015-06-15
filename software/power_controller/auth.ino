/*
Functions for authenticating users, timing out sessions,
fetching tools, logging tool use.
*/
bool auth_user(String rfid)
{
    Serial.print(F("user auth for "));
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
        Serial.print(F("name:")); Serial.println(user.name);
        int i = 0;
        while(p.available() > 1) //more than one means we skip the \n
        {
            user.tools[i] = p.parseInt();
            Serial.print(F("tool:")); Serial.println(user.tools[i]);
            i++;
        }
    }

    return(user.name != "");
}

int get_tools()
{
    Serial.println(F("fetch tools"));
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
        //read all the tools or until MAX_TOOLS is reached
        while(p.available())
        {
            //name
            tools[num_tools].name = p.readStringUntil(',');
            //read the tool's id and whether it's working at the moment
            tools[num_tools].id = p.parseInt();
            tools[num_tools].operational = p.parseInt();
            //chomp the newline
            p.read();

            Serial.print(F("tool id:")); Serial.print(tools[num_tools].id);
            Serial.print(F(" name:")); Serial.println(tools[num_tools].name);
            num_tools++;
            if(num_tools >= MAX_TOOLS)
                break;
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
    tools[page_num].current_rfid = user.rfid;
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
    p.addParameter(tools[page_num].current_rfid);
    p.addParameter("--time");
    p.addParameter(lcd_format_time(tools[page_num].time));
    p.run();
}

//call periodically to keep our tool table up to date
bool can_update_tools()
{
    //don't update if someone logged in
    if(user.name != "")
        return false;

    //don't update if a tool is in use
    for(int i = 0; i < num_tools; i++)
    {
        if(tools[i].running)
            return false;
    }   

    return true;
}
