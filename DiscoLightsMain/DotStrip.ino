#define DEBUG 1

    DotStrip::DotStrip (int throwaway)
    {
          dataPin=4;
          clockPin=5;
          numPixels=NUMPIXELS;
          pinMode(dataPin, OUTPUT);
          pinMode(clockPin,OUTPUT);       
          off(); //turn it all off now, but initialise the brightness levels;
          show();
    }

    int DotStrip::Pixels (void)
    {
      return numPixels;
    }


/*This turns on a pixel inside the array with the specified amount etc.*/
/*Order of Pixels on DotStar is BLUE,GREEN,RED*/

    void DotStrip::turnon(int Pixel, int Red, int Green, int Blue)
    {
      unsigned long temp;
      if (Pixel>=numPixels) {return;} //Array Out of Bounds Error Ignored.
      LEDARRAY[Pixel][0] = LEDARRAY[Pixel][0]= (unsigned char) Red;
      LEDARRAY[Pixel][1] = LEDARRAY[Pixel][1]= (unsigned char) Green;
      LEDARRAY[Pixel][2] = LEDARRAY[Pixel][2]= (unsigned char) Blue;
    }


//The top Byte of the Pixel Word controls brighness. The top three bits are always 1 as part of a protocol
//Leaves the bottom 5 bits to control brightness in range 0-31;

void DotStrip::setGlobalBrightnessByte(unsigned char Brightness)
{
  if (Brightness>B00011111) Brightness=B00011111;  // 5 Bits of Brightness;
  Brightness=Brightness|B11100000; // Set the top 3 bits to 1.
  for (int a=0;a<numPixels;a++)
  {
    LEDARRAY[a][3]=(unsigned char)Brightness;
  }
}

//Sets the Pixel Array to Zero Value;
void DotStrip::off()
{
  for (int a=0;a<numPixels;a++)
  {
    LEDARRAY[a][0]=0; //Set all bits to zero
    LEDARRAY[a][1]=0; //Set all bits to zero
    LEDARRAY[a][2]=0; //Set all bits to zero
    LEDARRAY[a][3]=0; //Set all bits to zero
  }
  setGlobalBrightnessByte(B00011111); //re-programme the top 8 bytes to be compliant.
}


//Send out the bits one by one through the array
void DotStrip::show(void)
{

int a;

  #ifdef DEBUG
      Serial.print(numPixels,DEC);
      Serial.println(" ARRAY SIZE - SEND STARTING");
  #endif
 // startTX();
 for (a=0;a<numPixels;a++)
 {

#ifdef DEBUG
      Serial.print("Array Element ");
      Serial.print(a,DEC);
      Serial.print(" Contains ");
      Serial.print(LEDARRAY[a][0],HEX);
      Serial.print("x");
      Serial.print(LEDARRAY[a][1],HEX);
      Serial.print("x");
      Serial.print(LEDARRAY[a][2],HEX);
      Serial.print("x");
      Serial.print(LEDARRAY[a][3],HEX);
      //Serial.print(" BIT ");
     // Serial.print(b,DEC);
      //Serial.print(" is ");
      //Serial.print(msb,BIN);
      Serial.println();
#endif
}
//  endTX();
//#ifdef DEBUG
      Serial.println("ARRAY SEND COMPLETE");
//#endif
}


    void DotStrip::sendBit(bool val)
    //Data is clocked out on the rising clock edge.
    {
      PORTD= (B00000000 | (val << 4) | B11101111);
      PORTD= (B00100000 | (val << 4) | B11100111);
    }

    void DotStrip::startTX()
    {
      for (int a=0; a<32; a++) sendBit(LOW);
    }
    
    void DotStrip::endTX()
    {
      for (int a=0; a<numPixels; a++) sendBit(LOW);
    }
