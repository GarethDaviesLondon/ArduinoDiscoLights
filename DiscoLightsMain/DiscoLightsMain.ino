
#include "DotStrip.h"
//#define DEBUG 1


DotStrip *ds;
  
void setup() {
#ifdef DEBUG
      Serial.begin(57600);     //Enable serial monitor line
      Serial.println("Hello I'm Alive");  
      Serial.println("Debug Enabled");
#endif
    ds = new DotStrip(1);
    ds->setGlobalBrightness(10);
}


void loop() {
  delay(500);
  int a =0;
  //ds->offAll();
  //for (a=0;a<NUMPIXELS;a++)
  //{
    ds->turnon(a,255,0,0);
    ds->turnon(a+1,0,255,0);
    ds->turnon(a+2,0,0,255);
    ds->turnon(a+3,255,255,255);
    ds->show();
  //}
  for (int b = 31; b>=0;--b)
  {
    ds->setGlobalBrightness(b);
    ds->show();
    delay(100);
  }
  
}









