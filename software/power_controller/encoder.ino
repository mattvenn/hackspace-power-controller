#include <Rotary.h>

Rotary r = Rotary(ENC_A, ENC_B);

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
