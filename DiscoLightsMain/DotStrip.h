#define NUMPIXELS 150 // Number of LEDs in strip maximum allowed defines array bounds, what is used is controlled by the variable numPix
#ifndef DOTSTRIPH
#define DOTSTRIPH

class DotStrip
{
  public:

    //Declare a DOTSTRIP object with a size upto the maximum number of PIXELS defined by NUMPIXELS constant
    DotStrip::DotStrip (int);
    DotStrip::DotStrip (int,int,int); //this enables the clock and data pin to be changed during object construction.

    //reports back on the length of the current strip
    int pixels(void);

    //enables re-sizing of the number of useable pixels in operation (up to maxium set by NUMPIXELS)
    void setPixels(int);

    //show() method is used to read the internal array out onto the DOTSTAR strip.
    void show();

    //hide() writes a null pattern turning everything off to the strip. It leaves the internal array in tact.
    void hide();

    //offAll() clears the internal array - still requires a show() command to be issued to write to array
    void offAll(void);

    //This turns off a single pixel.
    void offOne(int a);
   
    //These are used to instruct a pixel by giving it RGB input with an optional brightness. Don't forget to use the show() method to transmit
    void turnOn(int Pixel, byte Red, byte Green, byte Blue);
    void turnOn(int Pixel, byte Red, byte Green, byte Blue, byte Brightness);


   //Turns a pixels Red Green or Blue value to the desired value, leaves the other values untouched.
    void turnRed(int Pixel, byte Red);
    void turnGreen(int Pixel, byte Green);
    void turnBlue(int Pixel, byte Blue);


    //brightness() sets the brightness of an individual pixel - remember to use the show() method before this displays
    void brightness(int Pixel,byte Brightness);

    //setGlobalBrightness() sets the brightness for all pixels - remember to use the show() method before this displays
    void setGlobalBrightness(byte Brightness);


  private:
    int clockPin = 6; //The default clock pin on the Arduino
    int dataPin = 7;  //The default data pin on the Arduino
    int numPixels = NUMPIXELS; 
    int StripBrightness;
    byte LEDARRAY[NUMPIXELS][4];
    byte PortDLowMASK;
    byte PortDSendZERO;
    byte PortDSendONE;

    
    void ConstructBase(int); // A common constructore method, as I've forgotten how to overload properly!
    void startTX(); // Sends a startTX pattern
    void endTX();   //Sends an end of transmission pattern
    void testPins();  //A method that sends data to pins to make sure that you've wired things up right!

};

#endif

