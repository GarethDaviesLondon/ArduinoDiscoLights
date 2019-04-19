#ifndef SEQUENCEH
#define SEQUENCEH

#define REDINCREMENT 10
#define NUMPIXELS 144
#define MIDSCALER 10
#define ENDSCALER 10
#define INTESITYSCALER 10
#define BASEINTENSITY 30
#define COMMLED 9

#define SEQUENCEGATEtop 75
#define SEQUENCEGATEmid 30
#define SEQUENCEGATEbass 170

#define SEQUENCESTARTSCALEtop 150
#define SEQUENCESTARTSCALEmid 150
#define SEQUENCESTARTSCALEbass 200

#define SEQUENCEFULLSCALEtop 250
#define SEQUENCEFULLSCALEmid 400
#define SEQUENCEFULLSCALEbass 350

#define SAMPLEWINDOW 64
#define CALIBRATIONWINDOW 256

#ifndef EEPROMGLOBALS

//These are used to address EEPROM memory.
//Note that these are integer values therefore require two bytes so EEPROM index jumps in 2s.
#define CALCONFIRM 3
#define CALVU1MIN 4
#define CALVU1AV 6
#define CALVU1PEAK 8
#define CALVU2MIN 10
#define CALVU2AV 12
#define CALVU2PEAK 14
#define CALVU3MIN 16
#define CALVU3AV 18
#define CALVU3PEAK 20

//Used to store the number of LEDS
#define LEDSIZE 22

#endif

#include "DotStrip.h"


class Sequence
{
  public:
  
  Sequence::Sequence(DotStrip*);
  
  void superFlash (int);
  void strobeWhite (int);
  void strobeColour(int,int);
  void redBuild();
  void colourMix();
  void communicate(int);
  void calibrate(void);
  bool isCalibrated(void);
  void showBass();
  void showMid();
  void showTreble();
  void showBass(int);
  void showMid(int);
  void showTreble(int);
  void showAllChannels(int);
  void goDark();
  void rainbowMix();
  void randomMix();
  void channelMovesRed();
  void mixItUp();
  void writeEPint(int addr, int inp);
  int readEPint(int addr);
  void calibrationSample(void);
  void calibrationSample(int);

//private:

  DotStrip *ds;
  int TrebleAnalogPin = A3;
  int BassAnalogPin = A2;
  int MidAnalogPin = A1;
  int vu1Av,vu2Av,vu3Av=0;  
  int vu1Peak,vu2Peak,vu3Peak=0;
  int vu1Min,vu2Min,vu3Min=1024;
  int vu1Sam,vu2Sam,vu3Sam=0;
  int vu1Scale,vu2Scale,vu3Scale;
  int GlobalBrightness=5; 
  int flashtrigger;
  
  
  void sample();

  void loadCalibrations(void);
  void rainbow (int);
  void inverseRainbow(int);
  void midRainbow(int);
  void randomColour(int,int);
  void inverseRandomColour(int,int);
  void midRandomColour(int,int);
  void barGraph(int,int,int,int);
  void inverseBarGraph(int,int,int,int);
  void midBarGraph(int,int,int,int);
  void lightsRight(int,int);
  void lightsLeft(int,int);
  void lightsMid(int,int);

};

#endif

