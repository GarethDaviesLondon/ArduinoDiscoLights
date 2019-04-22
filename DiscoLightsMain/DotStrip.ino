/*
 * DOTSTRIP - Object to control a dotstar LED strip.
 * By Gareth Davies, Brighton, March 2019
 * This is used as part of the disco light project for Afrika Burn 2019
 * 
 * 
 * DOTSTAR - (APA102C) https://cdn-shop.adafruit.com/datasheets/APA102.pdf
 * 
 * This works by manipulating data on the SDI and CKI pins (Serial Data In, Clock In)
 * 
 * Each LED has a 32 FRAME 
 * MSB->LSB
 * Byte 4 [MSByte] = 111 (always 3x 1) + 5bits which is a global 0-31 (DEC) 00000-11111 range of brightness
 * Byte 3 BLUE
 * Byte 2 GREEN
 * Byte 1 RED
 * 
 * Bytes 3,2,1 are 0-255 level of brightness for each colour component of the LED array.
 * 
 * A data transmission consists of : "0" transmitted for 32 Bits (Start Word) + N*32 LED Words + 1 transmitted for 32 Bits (Stop Word).
 * 
 * 
 * There is an internal array of 32bit words, each word relating to one LED in the DOTSTAR
 * this is defined in the static LEDARRAY
 * 
 * Setting the RGB+Brightness for each LED in the internal array is done through the methods here
 * show() method is used to bit-bang it out on the Serial line.
 * 
 * The digital output pins for data and clock are defind in the static data of the Object and can be changed
    int clockPin = 6;
    int dataPin = 7;
 * 
 */


//#define DEBUGDOTSTRIP  // Uncomment this line to get the Object to spew out loads of stuff if you're debugging and think it has failed!


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
          pinMode(dataPin, OUTPUT); //set the mode for the digital pins
          pinMode(clockPin,OUTPUT);   

          
          //Set up the bit masks for the data sending later in the code base
          PortDSendZERO = 0x01 << clockPin; //Move the 1 in the byte to the clock position 
          PortDSendONE = PortDSendZERO | 0x01 << dataPin; // Sets the two bits high by adding in the data mask
          PortDLowMASK = 0XFF ^ PortDSendONE;  //XOR will set the two bits to zero, everything else 1    
          
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
          
          offAll(); //turn it all off now, but initialise the brightness levels;
          show();          

    }

void DotStrip::testPins() //Used to make sure that the clock and data pins are responding
                          //Needs a scope/logic analyser connected to see it though
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


//Used to check the size of the active array.
int DotStrip::pixels(void)
{
      return (numPixels);
}

//Used to set the size of the active array.
//Uses bounds checking to make sure that things are not over specified.
void DotStrip::setPixels(int numP)
{
  
     if ( (numP > NUMPIXELS) | (numP < 0) )
          {
            numPixels = NUMPIXELS;
          }
          else
          {
            numPixels=numP;
          }
 }

////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////



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

//Turn off a single pixel
void DotStrip::offOne(int a)
{

    if ((a<0)||(a>numPixels)) return;
    LEDARRAY[a][1]=0; //Set all bits to zero
    LEDARRAY[a][2]=0; //Set all bits to zero
    LEDARRAY[a][3]=0; //Set all bits to zero

}

//This sends a null set of data to the DotStar array to turn it dark
//But leaves the internal array untouched
//Can be used to toggle a complex pattern for example, without having to re-do everything.

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

 startTX(); //SEND START OF TRANSMISSION SIGNAL
 
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

  for (by=0;by<4;by++) // Each element in the LED array contains 4 bytes (a 32 bit word), each bit needs to be sent!
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

      tmpByte=mByte;
      
      //This loops over every 8 bits in the byte and shits out the data
      //serially sending a one or zero together with clock data
      //for each bit in the byte.
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
  }
 }
 endTX(); //SEND END OF TRANSMISSION SIGNAL
 
#ifdef DEBUGDOTSTRIP 
      Serial.println("ARRAY SEND COMPLETE");
#endif
}

//Send start transmission sequence
void DotStrip::startTX()
    {
      for (int a=0; a<32; a++) 
      {
                PORTD = PORTD & PortDLowMASK;
                PORTD = PORTD | PortDSendZERO;
      }
    }

//Send stop bits to strip
void DotStrip::endTX()
    {
      for (int a=0; a<numPixels; a++) 
      {
           PORTD = PORTD & PortDLowMASK;
           PORTD = PORTD | PortDSendZERO;
      }
    }

