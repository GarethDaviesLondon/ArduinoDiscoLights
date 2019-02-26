
#include "DotStrip.h"
#define NUMPIXELS 144 // Number of LEDs in strip

DotStrip ds = new DotStrip(NUMPIXELS);
  
void setup() {

  pinMode(LED_BUILTIN,OUTPUT);
  delay(1000);
  ds.turnon(1,255,0,0);
  ds.show();


}



void loop() {
}

/*

SIGNAL (TIMER0_COMPA_vect)
{
  interuptNum++;
  if (interuptNum < 100)
  {
    digitalWrite(LED_BUILTIN,HIGH);
  }
  else
  {
    if (interuptNum <200)
    {
      digitalWrite(LED_BUILTIN,LOW);
    }
    else
    {
      interuptNum=0;
    }
  }
}



void lightgraph(int bLevel)
{
  bargraph=bLevel;
  StartTX();
  for (int a=0;a<NUMPIXELS-bLevel;a++)
  {
    clearPixel();
  }
  for (int a=bLevel;a>0;a--)
  {
    commandPixel(1,0,0);
  }
  EndTX();
}

*/












