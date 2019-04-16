
#include "DotStrip.h"
#include "ShaftEncoder.h"
#include "Sequence.h";
#include "DiscoLights.h"

DotStrip *ds;
Sequence *pattern;

void setup() {
  
    initShaftEncoder(0,shaftMAX); //this sets the limits on the shaft encoder;
    ds = new DotStrip(72);
    pattern = new Sequence (ds);
    
#ifdef DEBUGMAIN
      Serial.begin(57600);     //Enable serial monitor line
      Serial.println("Hello I'm Alive");  
      Serial.println("Debug Enabled");
#endif
}


void loop ()
{
  if (shaftRebootFlag == true )
  {
    shaftRebootFlag=false;
#ifdef DEBUGMAIN
Serial.println("Changing to sequence");
Serial.println(shaftCounter);
#endif
  }

if (shaftLongPressFlag == true )
  {
    shaftLongPressFlag=false;
    if (shaftCounter==0) // Long Press in the far left condition gives a recalibration sequence;
    {
        #ifdef DEBUGMAIN
        Serial.println("Initiating calibration sequence");
        #endif
          pattern->calibrate();
        #ifdef DEBUGMAIN
        Serial.println("Calibration Complete");
        #endif
    }
    if (shaftCounter==shaftMAX) //At far Right end, then we can toggle between various maximum LED lengths;
    {
        NULL;
    }
  }
  
#ifdef DEBUGMAIN
static int lastShaft = -1;
#endif

  
  switch (shaftCounter)
  {

    /*
    case 0:  
        pattern->groovy();
        break;
         
    case 1: 
        pattern->boogie();
        break;
    case 2:
        pattern->superFlash();
        break;
    case 3:
        pattern->showVolts();
        break;
        */
    case 0:
     #ifdef DEBUGMAIN
        if (shaftCounter!= lastShaft) Serial.println("Going Dark");
        lastShaft=shaftCounter;
      #endif
        pattern->goDark();

    break;    
    case 1:
        #ifdef DEBUGMAIN
            if (shaftCounter!= lastShaft) Serial.println("Showing Bass");
            lastShaft=shaftCounter;
        #endif
        pattern->showBass();
        break;
    case 2:
        #ifdef DEBUGMAIN
            if (shaftCounter!= lastShaft) Serial.println("Showing Mid Range");
            lastShaft=shaftCounter;
        #endif
        pattern->showMid();
        break;
    case 3:
        #ifdef DEBUGMAIN
            if (shaftCounter!= lastShaft) Serial.println("Showing Treble");
            lastShaft=shaftCounter;
        #endif
        pattern->showTreble();
        break;
    case 4:
         #ifdef DEBUGMAIN
            if (shaftCounter!= lastShaft) Serial.println("Showing MidRange");
         #endif
        lastShaft=shaftCounter;

        pattern->showAllChannels();
        break;

    default:   
       delay(5000);
#ifdef DEBUGMAIN
Serial.println("Default");
#endif
        break;

  }
}

