#define NUMPIXELS 144 // Number of LEDs in strip
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
  private:

    int StripBrightness;
    int clockPin;
    int dataPin;
    byte LEDARRAY[NUMPIXELS][4];
    void sendBit(bool val);
    void startTX();
    void endTX();
};

#endif
