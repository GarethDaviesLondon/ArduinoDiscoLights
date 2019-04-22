#define NUMPIXELS 150 // Number of LEDs in strip maximum allowed defines array bounds, what is used is controlled by the variable numPix
#ifndef DOTSTRIPH
#define DOTSTRIPH

class DotStrip
{
  public:
    DotStrip::DotStrip (int);
    void turnOn(int Pixel, byte Red, byte Green, byte Blue);
    void turnOn(int Pixel, byte Red, byte Green, byte Blue, byte Brightness);
    void turnRed(int Pixel, byte Red);
    void turnGreen(int Pixel, byte Green);
    void turnBlue(int Pixel, byte Blue);
    void brightness(int Pixel,byte Brightness);
    void show();
    void hide();
    void setGlobalBrightness(byte Brightness);
    void offAll(void);
    void offOne(int a);
    int pixels(void);
    void setPixels(int);

    
  private:
  
    int clockPin = 6;
    int dataPin = 7;
    int numPixels = NUMPIXELS;
    int StripBrightness;
    byte LEDARRAY[NUMPIXELS][4];
    byte PortDLowMASK;
    byte PortDSendZERO;
    byte PortDSendONE;
    void startTX();
    void endTX();
    void testPins();

};

#endif

