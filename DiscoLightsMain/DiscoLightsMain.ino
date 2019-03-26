
#include "DotStrip.h"
#include "ShaftEncoder.h"
#include "Sequence.h";

#define DEBUGMAIN

DotStrip *ds;
Sequence *pattern;

void setup() {

#ifdef DEBUGMAIN
      Serial.begin(57600);     //Enable serial monitor line
      Serial.println("Hello I'm Alive");  
      Serial.println("Debug Enabled");
#endif
    initShaftEncoder(0,5); //this sets the limits on the shaft encoder;
    ds = new DotStrip(72);
    pattern = new Sequence (ds);
}


void loop ()
{

  switch (shaftCounter)
  {
    case 0:  
        Serial.println("Executing Pattern 0");
        pattern->groovy();
        break;
    case 1: 
        Serial.println("Executing Pattern 1");
        pattern->boogie();
        break;
    case 2:
        Serial.println("Executing Pattern 2");
        pattern->superFlash();
        break;
    default:
       Serial.println("Sorry no idea what to do");
       delay(5000);
       break;
  }
}

