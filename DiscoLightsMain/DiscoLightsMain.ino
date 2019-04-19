
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
