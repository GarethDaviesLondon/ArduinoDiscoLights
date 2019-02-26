class DotStrip
{
  public:
    DotStrip::DotStrip (int n);
    int Pixels (void);
    void commandPixel(unsigned int red,unsigned int green, unsigned int blue);
   
  private:

    int numPixels;
    void sendBit(bool val);
    void StartTX();
    void EndTX();
    void wipe();
    void clearPixel();
    void sendByteHigh();
    void sendByteLow();
    void turnon(int Pixel, int Red, int Green, int Blue);
};

