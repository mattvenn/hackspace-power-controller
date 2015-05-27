void check_encoder()
{
  AdaEncoder *thisEncoder=NULL;
  thisEncoder=AdaEncoder::genie();
  if (thisEncoder != NULL) {
    clicks=thisEncoder->query();
    if (clicks > 0) {

      count += 1;
      Serial.println(count);
      
    }
    if (clicks < 0) {

       count -= 1;
             Serial.println(count);
    }
  }
}
