#include <Rotary.h>

Rotary r = Rotary(ENCA_a, ENCA_b);

void check_encoder()
{
  unsigned char result = r.process();
  if (result) {
    if(result == DIR_CW)
        enc_clicks ++;
    else
        enc_clicks --;
  }
}
