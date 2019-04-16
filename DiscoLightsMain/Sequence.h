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

#ifndef EEPROMGLOBALS

//These are used to address EEPROM memory.
//Note that these are integer values therefore require two bytes so EEPROM index jumps in 2s.
#define CALCONFIRM 3
#define CALVU1MIN 4
#define CALVU1AV 6
#define CALVU1PEAK 8
#define CALVU2MIN 10
#define CALVU2AV 22
#define CALVU2PEAK 14
#define CALVU3MIN 16
#define CALVU3AV 18
#define CALVU3PEAK 20
#endif

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
  void showVolts(void);
  void calibrate(void);
  bool isCalibrated(void);
  void showCalibratedVolts(void);
  void barGraph(int level,int,int,int);
  void showBass();
  void showMid();
  void showTreble();
  void showAllChannels();
  void goDark();

  
  
  private:
  
  int TrebleAnalogPin = A3;
  int BassAnalogPin = A2;
  int MidAnalogPin = A1;
  
  int intensity = 30;
  unsigned char mainRED=128;
  unsigned char mainBLUE=128;
  unsigned char mainGreen=128;
  unsigned char mainBRIGHTNESS=16;


  int vu1Av,vu2Av,vu3Av=0;  
  int vu1Peak,vu2Peak,vu3Peak=0;
  int vu1Min,vu2Min,vu3Min=1024;
  int calVU1min,calVU1av,calVU1peak;
  int calVU2min,calVU2av,calVU2peak;
  int calVU3min,calVU3av,calVU3peak;
  
  int SizeRed=0;
  int SizeBlue=0;
  int GlobalBrightness=5; 
  bool Update = false;
  void sample(unsigned int);
  void calibrationSample();
  void printVoltage(int,int,int);
  void loadCalibrations(void);
  
  void Sequence::writeEPint(int addr, int inp);
  int Sequence::readEPint(int addr);
  bool checkBoot(void);
  DotStrip *ds;
  unsigned char RED,GREEN,BLUE;
};

#endif

