#define NUMPIXELS 144 // Number of LEDs in strip
class DotStrip
{
  public:
    DotStrip::DotStrip (int);
    int Pixels (void);
    void turnon(int Pixel, int Red, int Green, int Blue);
    void turnon(int Pixel, int Red, int Green, int Blue, int Brightness);
    void brightness(int Pixel,int Brightness);
    void show();
    void setGlobalBrightnessByte(unsigned char Brightness);
    void off(void);

  private:

    int numPixels;
    int clockPin;
    int dataPin;
    unsigned char LEDARRAY[NUMPIXELS][4];
    void sendBit(bool val);
    void startTX();
    void endTX();
};

