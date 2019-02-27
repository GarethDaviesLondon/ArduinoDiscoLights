
#include "DotStrip.h"
//#define DEBUG 1


DotStrip *ds;
int timer1_counter;
  
void setup() {
#ifdef DEBUG
      Serial.begin(57600);     //Enable serial monitor line
      Serial.println("Hello I'm Alive");  
      Serial.println("Debug Enabled");
#endif
    ds = new DotStrip(1);
    ds->setGlobalBrightness(1);

  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  // Set timer1_counter to the correct value for our interrupt interval
  //timer1_counter = 64911;   // preload timer 65536-16MHz/256/100Hz
  //timer1_counter = 64286;   // preload timer 65536-16MHz/256/50Hz
  timer1_counter = 34286;   // preload timer 65536-16MHz/256/2Hz
  
  TCNT1 = timer1_counter;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts
}


void loop() {
  delay(500);
  int a =0;
  for(a=0;a<NUMPIXELS;a++)
  {
    ds->turnon(a,255,0,0);
    ds->offOne(a-1);
    delay(100);
  }
  ds->offOne(NUMPIXELS-1);
  //ds->offAll();
  //for (a=0;a<NUMPIXELS;a++)
  //{
  //  ds->turnon(a,255,0,0);
  //  ds->turnon(a+1,0,255,0);
  //  ds->turnon(a+2,0,0,255);
  //  ds->turnon(a+3,255,255,255);
  //}
  //for (int b = 31; b>=0;--b)
  //{
  //  ds->setGlobalBrightness(b);
  // delay(100);
  //}
  
}
ISR(TIMER1_OVF_vect)        // interrupt service routine 
{
  TCNT1 = timer1_counter;   // preload timer
  ds->show();
}







