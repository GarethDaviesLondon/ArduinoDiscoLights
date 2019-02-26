class DotStrip
{
  public:
    DotStrip::DotStrip (int n);
    int Pixels (void);
    void turnon(int Pixel, int Red, int Green, int Blue);
    void turnon(int Pixel, int Red, int Green, int Blue, int Brightness);
    void brightness(int Pixel,int Brightness);
    void show();
    void setGlobalBrightnessByte(int Brightness);
    void Off(void);

  private:

    int numPixels;
    int clockPin;
    int dataPin;
    unsigned long LEDARRAY[];
    void sendBit(bool val);
    void StartTX();
    void EndTX();
};

