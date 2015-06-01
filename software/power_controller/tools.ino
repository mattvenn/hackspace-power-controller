

// placeholder, will be fetched from google spreadsheet!
void setup_users()
{
    //0
    users[0].user_name = "matt";
    users[0].rfid = "04184A4FC6";

    //1
    users[1].user_name = "nic";
    users[1].rfid = "04184A38D8";

}


    
// placeholder, will be fetched from google spreadsheet!
void setup_tools()
{
    tools[0].tool_name = "lathe";
    int t0[] = {  1, 1 };
    memcpy(tools[0].users, t0, sizeof t0);

    tools[1].tool_name = "mill";
    tools[1].operational = false;
    int t1[] = { 0, 1 };
    memcpy(tools[1].users, t1, sizeof t1);

    tools[2].tool_name = "laser";
    tools[2].operational = true;
    int t2[] = { 0, 1 };
    memcpy(tools[2].users, t2, sizeof t2);

    tools[3].tool_name = "saw";
    int t3[] = { 0, 0 };
    memcpy(tools[3].users, t3, sizeof t3);
}
