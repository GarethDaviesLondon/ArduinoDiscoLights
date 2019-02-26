
#include "DotStrip.h"
#define DEBUG 1


DotStrip *ds;
  
void setup() {
      Serial.begin(57600);     //Enable serial monitor line
      Serial.println("Hello I'm Alive");  
#ifdef DEBUG
      Serial.println("Debug Enabled");
#endif
    ds = new DotStrip(1);
  //ds.turnon(1,255,0,0);
  //ds.show();


}


void loop() {
  delay(500);
  //Serial.println("Hello I'm Looping");
  /*
  ds.off();
  for (int a=0;a<NUMPIXELS;a++)
  {
    ds.off();
    ds.turnon(a,255,0,0);
    ds.show();
  }
  */
}









