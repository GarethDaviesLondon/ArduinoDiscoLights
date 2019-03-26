
#include "DotStrip.h"
#include "ShaftEncoder.h"
#include "Sequence.h";

#define DEBUGMAIN
//#define DEBUGISR

#ifdef DEBUGISR
int Counter=0; // This is a counter to confirm the ISR is firing correctly.
#endif

#define CHAN1 0
#define CHAN2 1
#define REDINCREMENT 10
#define NUMPIXELS 144
#define MIDSCALER 10
#define ENDSCALER 10
#define INTESITYSCALER 10
#define BASEINTENSITY 30

DotStrip *ds;
Sequence *pattern;

int timer1_counter;
int BassAnalogPin = A7;
int MidAnalogPin = A6;
int TrebleAnalogPin = A5;

int intensity = 30;
unsigned char mainRED=128;
unsigned char mainBLUE=128;
unsigned char mainGreen=128;
unsigned char mainBRIGHTNESS=16;
int SizeRed=0;
int SizeBlue=0;

bool Update = false;

void setup() {


/////////////////////////////SET UP PINS

pinMode (BassAnalogPin,INPUT);
pinMode (MidAnalogPin,INPUT);
pinMode (TrebleAnalogPin,INPUT);


#ifdef DEBUGMAIN
      Serial.begin(57600);     //Enable serial monitor line
      Serial.println("Hello I'm Alive");  
      Serial.println("Debug Enabled");
#endif

    initShaftEncoder(0,5);
    ds = new DotStrip(1);
    pattern = new Sequence (ds);
    ds->setGlobalBrightness(5);
    ds->offAll();
    ds->show();
}


void loop ()
{

  
  /*
  //noInterrupts();
  int vu1 = analogRead(TrebleAnalogPin);
  int vu2 = analogRead(BassAnalogPin);
  int vu3 = vu1-vu3;
  
  int scale1 = vu1*ENDSCALER;
  int scale2 = vu2*MIDSCALER;
  int mid1 = (NUMPIXELS/2) + scale2 -(MIDSCALER*2);
  int mid2 = (NUMPIXELS/2) - scale2 +(MIDSCALER*2);
  int topscale = NUMPIXELS - scale2;
  int a;

  intensity = BASEINTENSITY+(scale1*2);
  
  for ( a=0;a<scale1;a++) {
    ds->turnGreen(a,intensity);
    ds->turnGreen(NUMPIXELS-a,scale1*10);
  }

  intensity=BASEINTENSITY+(scale2*10);
  for (a=0;a<scale2;a++)
  {
    ds->turnRed(mid1+a,intensity);
    ds->turnRed(mid2-a,intensity);
  }

  
  intensity = scale1 * scale2;
  
  for (a=0;a<NUMPIXELS;a++)
  {
    ds->turnBlue(a,intensity);
  }
  
  ds->setGlobalBrightness(INTESITYSCALER*vu3);
  ds->show();
  delay(20);
  ds->offAll();
  ds->show();

  */
}

