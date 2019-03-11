
#include "DotStrip.h"
//#include "TImerControl.h"

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
int timer1_counter;
int BassAnalogPin = A7;
int TrebleAnalogPin = A6;
int BassTrigger = 2;
int TrebleTrigger = 3;
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
pinMode (TrebleAnalogPin,INPUT);
pinMode (BassTrigger,INPUT);
pinMode (TrebleTrigger,INPUT);


#ifdef DEBUGMAIN
      Serial.begin(57600);     //Enable serial monitor line
      Serial.println("Hello I'm Alive");  
      Serial.println("Debug Enabled");
#endif
    ds = new DotStrip(1);
    ds->setGlobalBrightness(5);
    ds->offAll();
    ds->turnRed(2,128);
    ds->show();
    //configTimer();
    //attachInterrupt(CHAN1,Chan_1_On,RISING);
    //attachInterrupt(CHAN2,Chan_2_On,RISING);
}


void loop ()
{
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

  intensity=BASEINTENSITY+(scale2*5);
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
}

void Chan_1_On()
{
  noInterrupts();
  SizeRed+=REDINCREMENT;
  if (SizeRed<0) SizeRed=0;
  if (SizeRed>(NUMPIXELS/2)) SizeRed=NUMPIXELS/2;
  
  mainRED = 128;
  int a;
  
  
  for ( a=0;a<=SizeRed;a++)
  {
    ds->turnRed(a,mainRED);
  }
  for ( a=NUMPIXELS;a>=(NUMPIXELS-SizeRed);a--)
  {
    ds->turnRed(a,mainRED);
  }
  
  ds->show();
  attachInterrupt(CHAN1,Chan_1_Off,FALLING);
  interrupts();
}

void Chan_1_Off()
{
  noInterrupts();


  mainRED = 0;
  int a;

  for ( a=0;a<=SizeRed;a++)
  {
    ds->turnRed(a,mainRED);
  }
  
  for ( a=NUMPIXELS;a>=(NUMPIXELS-SizeRed);a--)
  {
    ds->turnRed(a,mainRED);
  }
  
  ds->show();
  SizeRed-=REDINCREMENT;
  if (SizeRed<0) SizeRed=0;
  if (SizeRed>NUMPIXELS) SizeRed=NUMPIXELS;
  
  attachInterrupt(CHAN1,Chan_1_On,RISING);
  interrupts();
}


void Chan_2_On()
{
  noInterrupts();
  mainBLUE = 128;
  int a;
  
  for (a=0;a<NUMPIXELS;a++)
  {
    ds->turnBlue(a,mainBLUE);
  }
  ds->show();
  attachInterrupt(CHAN2,Chan_2_Off,FALLING);
  interrupts();
}

void Chan_2_Off()
{
  noInterrupts();
  mainBLUE = 0;
  int a;
  
  for (a=0;a<NUMPIXELS;a++)
  {
    ds->turnBlue(a,mainBLUE);
  }
  ds->show();
  attachInterrupt(CHAN2,Chan_2_On,RISING);
  interrupts();
}


/*

v

*/

