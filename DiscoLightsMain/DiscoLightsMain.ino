
#include "DotStrip.h"
#include "ShaftEncoder.h"
#include "Sequence.h";

#define DEBUGMAIN

DotStrip *ds;
Sequence *pattern;

void setup() {
  
    initShaftEncoder(0,5); //this sets the limits on the shaft encoder;
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
  switch (shaftCounter)
  {
    case 0:  
        pattern->groovy();

        break;
        
    case 1: 
        pattern->boogie();
        break;
    case 2:
        pattern->superFlash();
        break;

    default:   
       delay(5000);
#ifdef DEBUGMAIN
Serial.println("Default");
#endif
        break;

  }
}

