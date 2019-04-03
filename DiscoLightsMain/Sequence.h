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
  void communicate(int);

  
  private:
  
  int TrebleAnalogPin = A3;
  int BassAnalogPin = A2;
  int MidAnalogPin = A1;
  
  int intensity = 30;
  unsigned char mainRED=128;
  unsigned char mainBLUE=128;
  unsigned char mainGreen=128;
  unsigned char mainBRIGHTNESS=16;
  int vu1Peak,vu2Peak,vu3Peak=0;
  int vu1Min,vu2Min,vu3Min=512;
  int SizeRed=0;
  int SizeBlue=0;
  int GlobalBrightness=5; 
  bool Update = false;
  int debugCounter=0;
  


  bool checkBoot(void);
  DotStrip *ds;
  unsigned char RED,GREEN,BLUE;
};

#endif

