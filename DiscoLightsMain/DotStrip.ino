//#define DEBUGDOTSTRIP
#define NORMALMODE

    DotStrip::DotStrip (int numP)
    {
          if ( (numP > NUMPIXELS) | (numP < 0) )
          {
            numPixels = NUMPIXELS;
          }
          else
          {
            numPixels=numP;
          }
          
          StripBrightness=31;
          pinMode(dataPin, OUTPUT);
          pinMode(clockPin,OUTPUT);       
          
          /*
           * In case you need to check the math, this shows that it works!
           * as you will see the HEX output is the same.
           * 
          PortDLowMASK=B11001111;
          PortDSendONE= B00110000;
          PortDSendZERO=B00100000;
          
          Serial.println(PortDLowMASK,HEX);
          Serial.println(PortDSendONE,HEX);
          Serial.println(PortDSendZERO,HEX);
          
          PortDSendZERO = 0x01 << 5; //Move the 1 in the byte to the clock position 
          PortDSendONE = PortDSendZERO | 0x01 << 4; // Sets the two bits high by adding in the data mask
          PortDLowMASK = 0XFF ^ PortDSendONE;  //XOR will set the two bits to zero, everything else 1    

          Serial.println(PortDLowMASK,HEX);
          Serial.println(PortDSendONE,HEX);
          Serial.println(PortDSendZERO,HEX);

          */

          //Set up the bit masks for the data sending later in the code base
          PortDSendZERO = 0x01 << clockPin; //Move the 1 in the byte to the clock position 
          PortDSendONE = PortDSendZERO | 0x01 << dataPin; // Sets the two bits high by adding in the data mask
          PortDLowMASK = 0XFF ^ PortDSendONE;  //XOR will set the two bits to zero, everything else 1

          //while(1) testPins();
          
          offAll(); //turn it all off now, but initialise the brightness levels;
          show();          

    }

void DotStrip::testPins()
{
         PORTD = PORTD & PortDLowMASK;
         delay(25);
         PORTD = PORTD | PortDSendONE;
         delay(25);
         PORTD = PORTD & PortDLowMASK;
         delay(25);
         PORTD = PORTD | PortDSendZERO;
         delay(25);
}


/*This turns on a pixel inside the array with the specified amount etc.*/
/*Order of Pixels on DotStar is BLUE,GREEN,RED*/


    void DotStrip::turnRed(int Pixel, byte Red)
    {
      if ((Pixel<0)||(Pixel>numPixels)) return;  
      LEDARRAY[Pixel][3]= Red;
      LEDARRAY[Pixel][0]|= B11100000; //This is a validation check the top three bits need to be HIGH for TX
    } 

    void DotStrip::turnGreen(int Pixel, byte Green)
    {
      if ((Pixel<0)||(Pixel>numPixels)) return;  
      LEDARRAY[Pixel][2]= Green;
      LEDARRAY[Pixel][0]|= B11100000; //This is a validation check the top three bits need to be HIGH for TX
    } 
  
    void DotStrip::turnBlue(int Pixel, byte Blue)
    {
      if ((Pixel<0)||(Pixel>numPixels)) return;  
      LEDARRAY[Pixel][1]= Blue;
      LEDARRAY[Pixel][0]|= B11100000; //This is a validation check the top three bits need to be HIGH for TX
    } 
  
    void DotStrip::turnOn(int Pixel, byte Red, byte Green, byte Blue)
    {
      if ((Pixel<0)||(Pixel>numPixels)) return;
      LEDARRAY[Pixel][3]= Red;
      LEDARRAY[Pixel][2]= Green;
      LEDARRAY[Pixel][1]= Blue;
      LEDARRAY[Pixel][0]|= B11100000; //This is a validation check the top three bits need to be HIGH for TX
    }


   void DotStrip::turnOn(int Pixel, byte Red, byte Green, byte Blue, byte Brightness)
    {
      if ((Pixel>=numPixels) || (Pixel<0)) {return;} //Array Out of Bounds Error Ignored.
      LEDARRAY[Pixel][2]= Green;
      LEDARRAY[Pixel][1]= Blue;
      LEDARRAY[Pixel][3]= Red;
      
      if (Brightness>B00011111) Brightness=B00011111;  // 5 Bits of Brightness;
      Brightness=Brightness|B11100000; // Set the top 3 bits to 1.
      LEDARRAY[Pixel][0]= Brightness;
    }

//The top Byte of the Pixel Word controls brighness. The top three bits are always 1 as part of a protocol
//Leaves the bottom 5 bits to control brightness in range 0-31;

void DotStrip::setGlobalBrightness(byte Brightness)
{

  if (Brightness>B00011111) StripBrightness=B00011111;  // 5 Bits of Brightness;
  StripBrightness=Brightness|B11100000; // Set the top 3 bits to 1.
  for (int a=0;a<numPixels;a++)
  {
    LEDARRAY[a][0]=(byte)StripBrightness;
  }

}


//Sets the Pixel Array to Zero Value;
void DotStrip::offAll()
{

  for (int a=0;a<numPixels;a++)
  {
    LEDARRAY[a][3]=0; //Set all bits to zero
    LEDARRAY[a][1]=0; //Set all bits to zero
    LEDARRAY[a][2]=0; //Set all bits to zero
    LEDARRAY[a][0]=B11100000; //Set all bits to zero
  }
}

void DotStrip::offOne(int a)
{

    if ((a<0)||(a>numPixels)) return;
    LEDARRAY[a][1]=0; //Set all bits to zero
    LEDARRAY[a][2]=0; //Set all bits to zero
    LEDARRAY[a][3]=0; //Set all bits to zero

}

void DotStrip::hide(void)
{
  
  byte tmp[4];
  byte tmpByte;
  tmp[1]=0;
  tmp[2]=0;
  tmp[3]=0;
  startTX();
  
  for (int a=0;a<numPixels;a++)
  {
    for (int b = 0; b<4;b++)
    {
      tmpByte=tmp[b];
      if (b=0)
      {
        tmpByte=B11100000; //| LEDARRAY[a][0];
      }
      for (int bi=0;bi<8;bi++)
        {
          //Set clock & data bit low
          PORTD = PORTD & PortDLowMASK;
          if ( ( B10000000 & tmpByte) > 0 )   
          {
            PORTD = PORTD | PortDSendONE;
          }
          else
          {
            PORTD = PORTD | PortDSendZERO;
          }
          tmpByte<<1;
        }
    }
  }
  endTX();
  
}

//Send out the bits one by one through the array
void DotStrip::show(void)
{
 
int a,by,bi;
bool mBit;
byte mByte;
byte tmpByte;

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
        PORTD = PORTD & PortDLowMASK;
        if ( ( B10000000 & tmpByte) > 0 )   
        {
          PORTD = PORTD | PortDSendONE;
        }
        else
        {
          PORTD = PORTD | PortDSendZERO;
        }
        tmpByte<<1;
      }
#endif NORMALMODE
  }
 }
 endTX();
#ifdef DEBUGDOTSTRIP 
      Serial.println("ARRAY SEND COMPLETE");
#endif
}


void DotStrip::startTX()
    {
      for (int a=0; a<32; a++) 
      {
                PORTD = PORTD & PortDLowMASK;
                PORTD = PORTD | PortDSendZERO;
      }
    }
    
void DotStrip::endTX()
    {
      for (int a=0; a<numPixels; a++) 
      {
           PORTD = PORTD & PortDLowMASK;
           PORTD = PORTD | PortDSendZERO;
      }
    }
