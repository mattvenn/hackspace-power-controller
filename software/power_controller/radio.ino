#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

/*
These codes were transcribed from a scope screen.
I found more here http://pastebin.com/RgQ4VCyw
But only the on codes worked, the off codes didn't.
*/

char * on_codes[] = {
    "101001000110001010101100",
    "101011011010011000000101",
    "101000001100100101111110",
};

char * off_codes[] = {
    "101001110111110010111100",
    "101000010101101001000101",
    "101010111011000001011110",
};

void setup_radio() 
{
    mySwitch.enableTransmit(RADIO_TX);
    mySwitch.setProtocol(4);
    mySwitch.setRepeatTransmit(20);  
}

void radio_turn_on(int tool_id)
{
    //tool ids are 1 indexed, codes are 0 indexed
    if(tool_id - 1 < MAX_TOOLS)
        mySwitch.send(on_codes[tool_id-1]);
}

void radio_turn_off(int tool_id)
{
    //tool ids are 1 indexed, codes are 0 indexed
    if(tool_id - 1 < MAX_TOOLS)
        mySwitch.send(off_codes[tool_id-1]);
}
