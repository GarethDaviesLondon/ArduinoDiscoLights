

    DotStrip::DotStrip (int n)
    {
          numPixels=n;
    }

    int DotStrip::Pixels (void)
    {
      return numPixels;
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
    
    
    void DotStrip::turnon(int Pixel, int Red, int Green, int Blue)
    {
      StartTX();
      for (int a =0;a<Pixel;a++) commandPixel(0,0,0);
      commandPixel(Red,Green,Blue);
      for (int b=Pixel+1;b<NUMPIXELS;b++) commandPixel(0,0,0);
      EndTX();
    }


