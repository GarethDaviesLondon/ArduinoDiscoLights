#ifndef SEQUENCEH
#define SEQUENCEH

#define REDINCREMENT 10
#define NUMPIXELS 144
#define MIDSCALER 10
#define ENDSCALER 10
#define INTESITYSCALER 10
#define BASEINTENSITY 30

#include "DotStrip.h"


class Sequence
{
  public:
  Sequence::Sequence(DotStrip*);
  void groovy (void);
  void boogie (void);
  void superFlash (void);
  void strobeWhite (int);
  void strobeColour(int,int);
  void redBuild();

  
  private:
  int BassAnalogPin = A7;
  int MidAnalogPin = A6;
  int TrebleAnalogPin = A5;
  int intensity = 30;
  unsigned char mainRED=128;
  unsigned char mainBLUE=128;
  unsigned char mainGreen=128;
  unsigned char mainBRIGHTNESS=16;
  int SizeRed=0;
  int SizeBlue=0;
  int GlobalBrightness=5; 
  bool Update = false;


  
  DotStrip *ds;
  unsigned char RED,GREEN,BLUE;
};

#endif

