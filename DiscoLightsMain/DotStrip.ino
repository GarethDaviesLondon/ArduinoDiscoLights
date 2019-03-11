//#define DEBUGDOTSTRIP
//#define SLOWMODE
#define NORMALMODE


    DotStrip::DotStrip (int throwaway)
    {
          dataPin=4;
          clockPin=5;
          numPixels=NUMPIXELS;
          StripBrightness=31;
          pinMode(dataPin, OUTPUT);
          pinMode(clockPin,OUTPUT);       
          offAll(); //turn it all off now, but initialise the brightness levels;
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

      if ((Pixel<0)||(Pixel>NUMPIXELS)) return;
      LEDARRAY[Pixel][3]= (unsigned char) Red;
      LEDARRAY[Pixel][2]= (unsigned char) Green;
      LEDARRAY[Pixel][1]= (unsigned char) Blue;
    }


   void DotStrip::turnon(int Pixel, int Red, int Green, int Blue,int Brightness)
    {
      if ((Pixel>=numPixels) || (Pixel<0)) {return;} //Array Out of Bounds Error Ignored.
      LEDARRAY[Pixel][3]= (unsigned char) Red;
      LEDARRAY[Pixel][2]= (unsigned char) Green;
      LEDARRAY[Pixel][1]= (unsigned char) Blue;
      if (Brightness>B00011111) Brightness=B00011111;  // 5 Bits of Brightness;
      Brightness=Brightness|B11100000; // Set the top 3 bits to 1.
      LEDARRAY[Pixel][0]= (unsigned char) Brightness;
    }

//The top Byte of the Pixel Word controls brighness. The top three bits are always 1 as part of a protocol
//Leaves the bottom 5 bits to control brightness in range 0-31;

void DotStrip::setGlobalBrightness(unsigned char Brightness)
{

  if (Brightness>B00011111) StripBrightness=B00011111;  // 5 Bits of Brightness;
  StripBrightness=Brightness|B11100000; // Set the top 3 bits to 1.
  for (int a=0;a<numPixels;a++)
  {
    LEDARRAY[a][0]=(unsigned char)StripBrightness;
  }

}

//Sets the Pixel Array to Zero Value;
void DotStrip::offAll()
{

  for (int a=0;a<numPixels;a++)
  {
    LEDARRAY[a][0]=0; //Set all bits to zero
    LEDARRAY[a][1]=0; //Set all bits to zero
    LEDARRAY[a][2]=0; //Set all bits to zero
    LEDARRAY[a][3]=B11100000; //Set all bits to zero
  }
}

void DotStrip::offOne(int a)
{

    if ((a<0)||(a>NUMPIXELS)) return;
    LEDARRAY[a][1]=0; //Set all bits to zero
    LEDARRAY[a][2]=0; //Set all bits to zero
    LEDARRAY[a][3]=0; //Set all bits to zero

}

//Send out the bits one by one through the array
void DotStrip::show(void)
{

int a,by,bi;
bool mBit;
unsigned char mByte;
unsigned char tmpByte;

  #ifdef DEBUGDOTSTRIP
      Serial.print(numPixels,DEC);
      Serial.println(" ARRAY SIZE - SEND STARTING");
  #endif
 startTX();
 for (a=0;a<numPixels;a++)
 {

#ifdef DEBUGDOTSTRIP
      Serial.println();
      Serial.println("----------------------------");
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
      Serial.print(" Going into Byte Process Loop");
#endif DEBUG
  for (by=0;by<4;by++)
  {
      mByte=LEDARRAY[a][by];
#ifdef DEBUGDOTSTRIP
      Serial.println();
      Serial.print("Byte ");
      Serial.print(by,DEC);
      Serial.print(" Contains ");
      Serial.print(mByte,BIN);
      Serial.println(" Going into Bit Sending Loop");
#endif

#ifdef NORMALMODE
      tmpByte=mByte;
      for (bi=0;bi<8;bi++)
      {
        //Set clock & data bit low
        PORTD = PORTD & B11001111;
        if ( ( B10000000 & tmpByte) > 0 )   
        {
          PORTD = PORTD | B00110000;
        }
        else
        {
          PORTD = PORTD | B00100000;
        }
        tmpByte<<1;
      }
#endif NORMALMODE


#ifdef SLOWMODE

    for (bi=0;bi<8;bi++)
    {
       mByte=LEDARRAY[a][by];
       mByte=mByte<<bi;
       mByte=mByte&B10000000;
       mByte=mByte>>7;
       mBit=(bool)mByte;
#ifdef DEBUGDOTSTRIP
      Serial.print(" Bit:");
      Serial.print(bi,DEC);
      Serial.print(",");
      Serial.print(mBit);
#endif DEBUGSTRIP 
      //Set clock & data bit low
      PORTD = PORTD & B11001111;
      //Set data bit
      PORTD = PORTD | mBit<<4;
      //Bring up clock latch
      PORTD = PORTD | B00100000;
    }
#endif  
  }
 }
 endTX();
#ifdef DEBUGDOTSTRIP 
      Serial.println("ARRAY SEND COMPLETE");
#endif
}


void DotStrip::sendBit(bool val)
    //Data is clocked out on the rising clock edge.
    {
      //Set clock & data bit low
      PORTD = PORTD & B11001111;
      //Set data bit
      PORTD = PORTD | val<<4;
      //Bring up clock latch
      PORTD = PORTD | B00100000;
    }

void DotStrip::startTX()
    {
      for (int a=0; a<32; a++) 
      {
                PORTD = PORTD & B11001111;
                PORTD = PORTD | B00100000;
      }
    }
    
void DotStrip::endTX()
    {
      for (int a=0; a<numPixels; a++) sendBit(LOW);
    }
