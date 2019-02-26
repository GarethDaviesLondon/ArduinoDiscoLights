

    DotStrip::DotStrip (int n)
    {
          dataPin=4;
          clockPin=5;
          numPixels=n;
          unsigned long *LEDARRAY=new unsigned long [n];
          pinMode(dataPin, OUTPUT);
          pinMode(clockPin,OUTPUT);
          setGlobalBrightnessByte(32); //Full On
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
      LEDARRAY[Pixel]=0; //Set to zero;
      temp = (unsigned long) (Red & 0x00FF);
      LEDARRAY[Pixel] = LEDARRAY[Pixel] | temp; //Red is LSB
      temp = (unsigned long) (Green & 0x00FF);
      LEDARRAY[Pixel] = LEDARRAY[Pixel] | temp << 8;
      temp = (unsigned long) (Blue & 0x00FF); 
      LEDARRAY[Pixel] = LEDARRAY[Pixel] | temp << 16;
    }


//The top Byte of the Pixel Word controls brighness. The top three bits are always 1 as part of a protocol
//Leaves the bottom 5 bits to control brightness in range 0-31;

void DotStrip::setGlobalBrightnessByte(int Brightness)
{
  unsigned long temp; //32 bit word
  if (Brightness>31) Brightness=31;  // 5 Bits of Brightness;
  Brightness+=B11100000; // Set the top 3 bits to 1.
  temp=Brightness; //Set bottom 8 bits to 1
  temp=temp << 24; // Move the word to the top of the 32 bit word (now is BRIGHTNES followed by 24 Zeros)
  for (int a=0;a<numPixels;a++)
  {
    LEDARRAY[a]=LEDARRAY[a]&0x0FFF; //Set top 8 bits to zero;
    LEDARRAY[a]=LEDARRAY[a]|temp; // Set top 8 bits to be temp;
  }
}

//Sets the Pixel Array to Zero Value;
void DotStrip::Off()
{
  for (int a=0;a<numPixels;a++)
  {
    LEDARRAY[a]=0x0; //Set top 8 bits to zero;
  }
  setGlobalBrightnessByte(32);
}


//Send out the bits one by one through the array
void DotStrip::show(void)
{
  unsigned long mask = 0x8000; //Just the top bit;
  unsigned long msb;
  unsigned long test=0xF0F0;
  bool b;
  msb = (test & mask)>>31;
  b=bool(msb);
  
  
}




    void DotStrip::commandPixel(unsigned int red,unsigned int green, unsigned int blue)
    {
      //send GlobalContrl 
      sendByteHigh();
      if (red>0) {sendByteHigh();}else{sendByteLow();}
      if (green>0) {sendByteHigh();}else{sendByteLow();}
      if (blue>0) {sendByteHigh();}else{sendByteLow();} 
    }

    void DotStrip::sendBit(bool val)
    //Data is clocked out on the rising clock edge.
    {
      PORTD=B00000000 | (val << 4);
      PORTD=B00100000 | (val << 4);
    }

    void DotStrip::StartTX()
    {
      for (int a=0; a<32; a++) sendBit(LOW);
    }
    
    void DotStrip::EndTX()
    {
      for (int a=0; a<numPixels; a++) sendBit(LOW);
    }

    void DotStrip::wipe()
    {
      StartTX();
      for (int a=0;a<numPixels;a++)
      {
        clearPixel();
      }
      EndTX();
    }
    
    void DotStrip::clearPixel()
    {
        for (int b=0;b<3;b++) sendBit(HIGH);
        for (int b=0;b<29;b++) sendBit(LOW);
    }


     void DotStrip::sendByteHigh()
     {
      for (int a=0; a<8; a++)
      {
        sendBit(HIGH);
      }
    }
    
    void DotStrip::sendByteLow()
    {
      for (int a=0; a<8;a++)
      {
        sendBit(LOW);
      }
    }
    
    


