/*
 * This is the programme that was written to control lights at AFRIKA Burn April 2019
 * Written by Gareth Davies, Brighton England
 * 
 * Hardware diagrams are available at:
 * 
 * Hardware provides analogue audio preamp followed by analogue filtering to High,Mid and Low Freq sent to Analogue inputs on the Arduino.
 * 
 * Defined in Sequence (where sampling occurs) 
 *   
  int TrebleAnalogPin = A3;
  int BassAnalogPin = A2;
  int MidAnalogPin = A1;

  Samples are then taken over a window and peak detected before being scaled on based on a stored min-max range for the analogue signal and scaled between zero
  and the number of LEDs on the NeoPixel array.

  The min-max ranges can be automatically calibrated using the calibrate function, though in practice some tweaking seemed necessary to get an appropriate dynamic
  range.
  
  Various combinations of patterns are then created based on these readings and translated into an in-memory array managed by the DotStrip class.

  Writing out the array to the strip is handled by a call to DotStrip::show() - this handles the serial data calls to bit-bash the data out
  Using Clock and Data pins defined in dotstrip.h

  int clockPin = 6;
  int dataPin = 7;

  The size of the LED array is controlled by two parameters
  one is a compiler directive in DotStrip.h
  #define NUMPIXELS 150 // Number of LEDs in strip maximum allowed defines array bounds, what is used is controlled by the variable numPix

  This is used to size the array stored in memory.

  The other is a variable accessed by:
    int pixels(void);
    void setPixels(int);

  These set the amount of the array to use. This enables switching between LED strip sizes in real-time without recompiling.

  The hardware includes a ShaftEncoder with a push switch, this is connected across the D2,D3 interrupt pins, ground and Difital in D4

  This is defined in ShaftEncoder.h

  #define shaftPushSw 2
  #define shaftOutputA 3
  #define shaftOutputB 4

  This enables a switching between modes which changes the pattern.
  There is an interrupt flag which is set which helps control flow in the main loop.
  If a button is pressed then, depending on the position of the shaft when it was pressed, either a calibration sequence or LED size switch is instigated.

  To add more patterns to the array

  1. Create a method with in Sequence to create the pattern
  2. Increase the number of states allowed in the shaft encoder (or replace an existing one)
  #define shaftMAX 11 in DiscoLights.h
  3. Add another case to the switch in the code below
 *
 * 
 */



#include "DotStrip.h"
#include "ShaftEncoder.h"
#include "Sequence.h";
#include "DiscoLights.h"

DotStrip *ds;
Sequence *pattern;

void setup() {
  
    initShaftEncoder(0,shaftMAX); //this sets the limits on the shaft encoder;
    ds = new DotStrip(NUMPIXELS);
    pattern = new Sequence (ds);
    ds->setPixels(pattern->readEPint(LEDSIZE));
    if ((ds->pixels()>NUMPIXELS) || (ds->pixels()<0) ) {ds->setPixels(NUMPIXELS);}
    
#ifdef DEBUGMAIN
      Serial.begin(57600);     //Enable serial monitor line
      Serial.println("Hello I'm Alive");  
      Serial.println("Debug Enabled");
      Serial.print("Set for Pixels :");
      Serial.println(ds->pixels());
#endif
}


void loop ()
{
  commandline();
  
#ifdef DEBUGMAIN
  if (shaftInterruptOccurred == true)
  {
   //Serial.println("Interrupt has Occurred");
  }
  if (shaftRebootFlag == true )
  {
    Serial.println("Changing to sequence");
    Serial.println(shaftCounter);
    pattern->strobeColour(0,1);
    delay(100);
    pattern->strobeWhite(1);
  }
#endif

  shaftRebootFlag=false;
  shaftInterruptOccurred=false;

  if (shaftLongPressFlag == true )
    {
      shaftLongPressFlag=false;
      if (shaftCounter==0) // Long Press in the far left condition gives a recalibration sequence;
      {
          #ifdef DEBUGMAIN
          Serial.println("Initiating calibration sequence");
          #endif
          pattern->strobeColour(2,10);
          pattern->calibrate();
          delay(100);
          pattern->strobeWhite(1);
          #ifdef DEBUGMAIN
          Serial.println("Calibration Complete");
          #endif
      }
      if (shaftCounter==shaftMAX) //At far Right end, then we can toggle between various maximum LED lengths;
      {
          #ifdef DEBUGMAIN
                Serial.print("Changing light length was ");
                Serial.print(ds->pixels());
          #endif
          switch (ds->pixels())
          {
            case 144:
                ds->setPixels(72);
                break;
            case 72:
                 ds->setPixels(30);
                 break;
            case 30:
                 ds->setPixels(144);
                 break;
          }
          pattern->writeEPint(LEDSIZE,ds->pixels());
          pattern->strobeColour(3,ds->pixels()/10);
          delay(100);
          pattern->strobeColour(0,ds->pixels()/10);
          delay(100);
          pattern->strobeWhite(3);
          #ifdef DEBUGMAIN
                Serial.print(" Now ");
                Serial.println(ds->pixels());
          #endif   
      }
    } //If ShaftLong Counter
  

  static int lastShaft = -1;

  switch (shaftCounter)
  {
    case 0:
     #ifdef DEBUGMAIN
        if (shaftCounter!= lastShaft) Serial.println("Going Dark");
        lastShaft=shaftCounter;
      #endif
        pattern->goDark();

    break;    
    case 1:
        #ifdef DEBUGMAIN
            if (shaftCounter!= lastShaft) Serial.println("redBuild");
         #endif
        lastShaft=shaftCounter;
        pattern->redBuild();
        break;
        
    case 2:
        #ifdef DEBUGMAIN
            if (shaftCounter!= lastShaft) Serial.println("Showing Bass");
            lastShaft=shaftCounter;
        #endif
        pattern->showBass();
        break;
    case 3:
        #ifdef DEBUGMAIN
            if (shaftCounter!= lastShaft) Serial.println("Showing Mid Range");
            lastShaft=shaftCounter;
        #endif
        pattern->showMid();
        break;
    case 4:
        #ifdef DEBUGMAIN
            if (shaftCounter!= lastShaft) Serial.println("Showing Treble");
            lastShaft=shaftCounter;
        #endif
        pattern->showTreble();  
        break;
    case 5:
         #ifdef DEBUGMAIN
            if (shaftCounter!= lastShaft) Serial.println("Showing channelMovesRed");
         #endif
        lastShaft=shaftCounter;

        pattern->channelMovesRed();
        break;
        
    case 6:
         #ifdef DEBUGMAIN
            if (shaftCounter!= lastShaft) Serial.println("Showing colourMix");
         #endif
        lastShaft=shaftCounter;

        pattern->colourMix();
        break;
    case 7:
        #ifdef DEBUGMAIN
            if (shaftCounter!= lastShaft) Serial.println("Showing rainbowMix");
         #endif
        lastShaft=shaftCounter;

        pattern->rainbowMix();
        break;
    case 8:
        #ifdef DEBUGMAIN
            if (shaftCounter!= lastShaft) Serial.println("Showing randomMix");
         #endif
        lastShaft=shaftCounter;
        pattern->randomMix();
        break;
    case 9:
        #ifdef DEBUGMAIN
            if (shaftCounter!= lastShaft) Serial.println("superFlash");
         #endif
        lastShaft=shaftCounter;
        pattern->superFlash(1);
        delay(100);
        break;
     case 10:
             #ifdef DEBUGMAIN
            if (shaftCounter!= lastShaft) Serial.println("mixItUp");
         #endif
        lastShaft=shaftCounter;
        pattern->mixItUp();
        break;

        default:
      case 11:
         pattern->calibrationSample(1); //look at the base pin
         break;
#ifdef DEBUGMAIN
            if (shaftCounter!= lastShaft) {
              Serial.print(shaftCounter);
              Serial.println(" Default, going Dark and returning");
            }
#endif
        lastShaft=shaftCounter;
        pattern->goDark();
        break;

  }
}



void commandline(void)
{
        String instr;
        int num;
        int chars;

        //chars=Serial.available();
        if (Serial.available() <= 0 ) return NULL;
        Serial.print("Processing input : ");
        instr = Serial.readString();
        Serial.print(instr);
        Serial.print(" charlength ");
        Serial.print(instr.length());
        Serial.print(" Contents 0x");
        for (int a=0;a<instr.length();a++)
        {
          Serial.print(instr.charAt(a),HEX);
        }
        Serial.println();
          Serial.println("Current Settings");
          Serial.print("Volatile 1,2,3 ");
          Serial.print(pattern->vu1Sam);
          Serial.print(" scaled ");
          Serial.print(pattern->vu1Scale);
          Serial.print(":");
          Serial.print(pattern->vu1Min);
          Serial.print(",");
          Serial.print(pattern->vu1Peak);
          Serial.print("++");
          Serial.print(pattern->vu2Sam);
          Serial.print(" scaled ");
          Serial.print(pattern->vu2Scale);
          Serial.print(":");
          Serial.print(pattern->vu2Min);
          Serial.print(",");
          Serial.print(pattern->vu2Peak);
          Serial.print("++");
          Serial.print(pattern->vu3Sam);
          Serial.print(" scaled ");
          Serial.print(pattern->vu3Scale);
          Serial.print(":");
          Serial.print(pattern->vu3Min);
          Serial.print(",");
          Serial.print(pattern->vu3Peak);
          Serial.print("++");
          Serial.println();

          if (instr.equals("com\n") == true )
          { 

          Serial.println("OK");
          //We have a valid command comming.
            
            while (Serial.available() == false)
            {
              delay(1);
            }
          
          instr = Serial.readString();
          if (instr.equals("bmin") )
          {
            Serial.print("bass min =");
          }
            while (Serial.available() == false)
            {
              delay(1);
            }
            num=Serial.read();
            Serial.print("\nSetting Bass Min to ");
            Serial.println(num);
            return NULL;
          }
}
