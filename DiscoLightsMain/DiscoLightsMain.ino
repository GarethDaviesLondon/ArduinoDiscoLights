
#include "DotStrip.h"
#define DEBUGMAIN
//#define DEBUGISR

#ifdef DEBUGISR
int Counter=0; // This is a counter to confirm the ISR is firing correctly.
#endif


DotStrip *ds;
int timer1_counter;
unsigned char RED,GREEN,BLUE;
int BassAnalogPin = A7;
int TrebleAnalogPin = A6;
int BassTrigger = 2;
int TrebleTrigger = 3;

bool Update = false;
  
void setup() {
      RED=255;
      GREEN=0;
      BLUE=0;

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
    ds->setGlobalBrightness(1);
    //configTimer();
    attachInterrupt(0,printBase,RISING);
    attachInterrupt(1,printTreble,RISING);
}

void loop ()
{
  delay (1000);
}


void printBase()
{
  noInterrupts();
  int a,b;
  //a = analogRead(TrebleAnalogPin);
  b = analogRead(BassAnalogPin);
  Serial.print("Read : ");
  //Serial.print(a);
  Serial.print(" Bass : ");
  Serial.print(b);
  Serial.println();
  interrupts();
}


void printTreble()
{
  noInterrupts();
  int a,b;
  a = analogRead(TrebleAnalogPin);
  //b = analogRead(BassAnalogPin);
  Serial.print("Read : ");
  Serial.print(" Treble ");
  Serial.print(a);
  //Serial.print(" Bass : ");
  //Serial.print(b);
  Serial.println();
  interrupts();
}

/*

void loop() {
  int a =0;
  int b=0;
  int c=10;
  int trail=30;
  int loop=1;
  for (c=0;c<trail;c++)
  {
    ds->offAll();
    for(a=0;a<NUMPIXELS;a++)
    {
      stopTimerInterrupt();
      ds->turnon(a+trail,RED,GREEN,BLUE,31);
      ds->turnon(NUMPIXELS-a-trail,RED,GREEN,BLUE,31);
      for (b=0;b<c;b++)
      {
        ds->turnon(a+trail,RED,GREEN,BLUE,1);
        ds->turnon(NUMPIXELS-b,RED,GREEN,BLUE,1);
        if (loop==1) {if (RED++==0) if (GREEN++==0) BLUE++; }//if (BLUE=254) loop++;}
        if (loop==2) {if (GREEN++==0) if (BLUE++==0) RED++; }//if (RED=254) loop++;}
        if (loop==3) {if (BLUE++==0) if (RED++==0) GREEN++;}//if (GREEN=254) loop++;}
        if (loop==4) {if (BLUE++==0) if (GREEN++==0) RED++; }//if (RED=254) loop++;}
        if (loop==5) {if (GREEN++==0) if (RED++==0) BLUE++; }//if (BLUE=254) loop=1;}
      }
      ds->offOne(a-c+1);
      ds->offOne(NUMPIXELS+1-a);
      Update = true;
      startTimerInterrupt();
      if (loop==6) {
        loop=0;
        //flash();
      }
  
      loop++;
      }
  }
}

*/

void flash()
{
  for (int a=0;a<20;a++)
  {
    for (int b=0;b<NUMPIXELS;b++)
    {
      ds->turnon(b,255,255,255,3);
      ds->show();
    }
    ds->offAll();
  }
}

void configTimer()
{

/*
 * Timers available in Arduino Uno
 * Timer 0 - 8 bits used for Timers such as millis and delay.
 * Timer 1 - 16 bits used in Servo Library
 * Timer 2 - 8 bits used in the Tone Library
 * 
 * Control registers TCCRx, OCRx,TIMSKx, TIFRx
 * 
 */

//Timer Control Register TCCRx 
//Initialise
unsigned char TCCAByte=0;
unsigned char TCCBByte=0;
TCCR2A=0;
TCCR2B=0;

//Timer Clock Source: TCCRn controlled by CS02:0 control bits

//Timer Counting Sequence: TCCRx controlled by WGM01 and WGM00 control bits - this will influence the TOVx interrupt
//Simple there is no reset and the counter rolls up to max and overflows back to zero, setting the TOVx flag. WGMn2:0=0;
//Clear Timer on Match (CTC MODE) means the counter will tick up to value, then trigger a compare and then reset to zero. WGMn2:0=2;
//To get CTC to work requires the OCFnx Flag to be set.


//Bits 7:6 COM0nA1:0 Compare Match Mode = 1,0 -> Clear OCnA on Match [IN COMPARE OUTPUT MODE CONTROLLE DBY WGMn2:0
TCCAByte |= B10000000; //Set Port to Clear on Match by setting 1,0 in bits 7 and 6

//Bits 5:4 COM0nB:1:0 This controlls the actions on compare with the Output Compare Register B (OCBn)
TCCAByte |= B00000000; // Disconnect 0C0B by setting bits 5 and 4 to zero

//Bits 3:2 are not used, reserved for future expansion

//Bits 1:0 WGMx1:0 These are WaveForm Generation Mode , it is combined with a bit in TCCB to form 3 bits control.
//Most Significant Bit (MSB) is in TCCB
// 0,1,0 specifies CTC Mode
TCCAByte |= 00000010;

/////CONTROL REGISTER B
///Bits7:6 Force output compare, this is not relevant here.

//Bits 5:4 are reserved and not used
//Bit 3: Wave Form Generator MSB
TCCBByte |= 00000000; // OK Pointless operation, but Bit 3 of TCCRB is WGM02

//Bits 2,1,0 control the clock selection and prescaler.
//Clock Speed is 16 Mhz and can be divided by 1,8,64,256 or 1024 by setting value 1,2,3,4,5 (binary). 
//Value of 6&7 specify external clock
//Value of 0 stops the clock;

TCCBByte |= 00000001; //Divide by 1;
//TCCBByte |= 00000010; //Divide by 8;
//TCCBByte |= 00000011; //Divide by 64;
//TCCBByte |= 00000100; //Divide by 256;
//TCCBByte |= 00000101; //Divide by 1024;

/******************WRITE TO REGISTER*************/

TCCR2A=TCCAByte;
TCCR2B=TCCBByte;

/***********************************************/

//Output Compare Register OCRnx
//There are two registers A & B - both of which can be compared to the counter.
//The counter (TCNTx) is compared to these registers, when there is a match a (and interrupt is enabled) it will trigger interupt.
//The interrupt is triggered because the OCFnx becomes set to 0. This flag is cleared automatically when the interrupt handler is called
//As an alternative software can write a 1 to the bit location.

//Set this compare value to the number of counts of the pre-scaled clock you want to count.
//For instance if the clock is 16 Mhz and is divided by 64, each pulse will be 256*(1/16) uSec - i.e. 16 uSec.
//If the counter is set to 128 it will trigger a pulse every 2,048 uSec (approx 2ms).

OCR2A=128;
  
}



void startTimerInterrupt()
{
  //Timer Interrupt Mask Register TIMSKn
//This controls behaviour based on the status of the Flag Register
//Timer Interrupt Flag Register TIFRn (indicates pending timer interrupts)

//Mask Bits 7..3 reserved for future use
//Mask Bit 2 - Counter Output Compare Match B Interrupt (compares with OCRnB)
//Mask Bit 1 - Counter Output Compare Match A Interrupt (compares with OCRnA)
//Mask Bit 0 - Counter Overflow Interrupt Enable (when the counter reaches max value it triggers).

  TIFR2 &= B11111101;   //Clear any pending interrupts;
  TIMSK2 = 2;           //Re-enable the interrupt mask;
}

void stopTimerInterrupt()
{
  TIMSK2 =0;
}

ISR(TIMER2_COMPA_vect)        // interrupt service routine 
{
  if (Update==false) return;
  stopTimerInterrupt();
 #ifdef DEBUGISR
  Serial.print("Ahem ");
  Serial.println(Counter,DEC);
  Counter++;
 #endif
  ds->show();
  Update=false;
  startTimerInterrupt();
}







