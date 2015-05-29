#ifdef USE_ENCODER
#include <Rotary.h>
Rotary r = Rotary(ENCA_a, ENCA_b);

int clicks = 0;
void check_encoder()
{
  unsigned char result = r.process();
  if (result) {
    Serial.println(result == DIR_CW ? "Right" : "Left");
  }
}
#endif
