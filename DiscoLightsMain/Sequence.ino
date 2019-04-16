#include "Sequence.h"
#include "ShaftEncoder.h"

//#define SEQUENCEDEUG
#define SEQUENCEDEUGSAMPLE

Sequence::Sequence (DotStrip *dotin)
{
  pinMode (BassAnalogPin,INPUT);
  pinMode (MidAnalogPin,INPUT);
  pinMode (TrebleAnalogPin,INPUT);
  pinMode (COMMLED,OUTPUT);
  ds=dotin;
  ds->setGlobalBrightness(GlobalBrightness);
  ds->offAll();
  ds->show();
  //loadCalibrations();
}

/*
 * This is the main sample capture routine.
 * During calibration it averages a larger number of samples
 * The debug flag SEQUENCEDEUGSAMPLE turns on a lot of reporting
 * this makes it very slow, so only use when debugging!
 */

void Sequence::sample(unsigned int junkit)
{
  return NULL;
}

void Sequence::calibrationSample()
{

    int sampleset = SAMPLEWINDOW;
    int starttime,stoptime,endtime;
    int pinSequence[3];
    pinSequence[0]=TrebleAnalogPin;
    pinSequence[1]=BassAnalogPin;
    pinSequence[2]=MidAnalogPin;
    
    int vuSamples [sampleset];

    for (int inputPin=0;inputPin<3;inputPin++)
    {
      #ifdef SEQUENCEDEUGSAMPLE
      Serial.println("Sampling Now");
      Serial.print("Channel ");
      Serial.print(inputPin);
      Serial.print(" - Sample Window Size per channel: ");
      Serial.println(sampleset);
      starttime=millis();
      Serial.print("Starttime ");
      Serial.println(starttime);
      #endif 
          //Capture Samples
          for (int a = 0; a < sampleset; a++)
          {
              vuSamples[a] = analogRead(pinSequence[inputPin]);
          }
      
      #ifdef SEQUENCEDEUGSAMPLE
        stoptime=millis();
        Serial.print("Sampling stoptime ");
        Serial.println(stoptime);
        int elapsedMillis=stoptime-starttime;
        Serial.print("Elapsed Sampling Time ");
        Serial.println(elapsedMillis);
        float millisPerSample=((float) elapsedMillis/(float) sampleset)*3;
        Serial.print("Milli Seconds Per Sample ");
        Serial.println(millisPerSample);
        float sampFreq=1000/millisPerSample;
        Serial.print("Sampling Frequency (Hz)");
        Serial.println(sampFreq);
      #endif 
      
      
          long int vuTot=0; //This needs to be a long int so that it doesn't overflow past 64K limit of an Int.
          int vuPeak=0;
          int vuMin=1024; 
          int vuAv=0;
      
          //Process Samples for Max,Min & Mean
      
          for (int a = 0; a < sampleset; a++)
          {
              vuTot += vuSamples[a];
              if (vuPeak<vuSamples[a]) vuPeak=vuSamples[a];
              if (vuMin>vuSamples[a]) vuMin=vuSamples[a];;
          }
          vuAv=vuTot/sampleset;
      
          float vuSD=0;
          
          //Calculate the standard deviation
          for (int a=0;a<sampleset;a++)
          {
            vuSD += sq(vuSamples[a]-vuAv);;
          }
          
          vuSD=vuSD/sampleset;
          vuSD=sqrt(vuSD);
          int vuSDInt=(int)vuSD;
      
      #ifdef SEQUENCEDEUGSAMPLE
        endtime=millis();
        Serial.print("Processing stoptime ");
        Serial.print(endtime);
        elapsedMillis=endtime-starttime;
        Serial.print(" Elapsed milli sceconds ");
        Serial.println(elapsedMillis);
        Serial.print("min,max,av,sd,sdInt : ");
        Serial.print(vuMin);
        Serial.print(":");
        Serial.print(vuPeak);
        Serial.print(":");
        Serial.print(vuAv);
        Serial.print(":");
        Serial.print(vuSD);
        Serial.print(":");
        Serial.println(vuSDInt);
      #endif
    
        switch (inputPin)
       {
        case 0:
            if (vuAv+(2*vuSDInt) < vuPeak) { vu1Peak=vuAv+(2*vuSDInt); } else { vu1Peak=vuPeak;}
            if (vuAv-(2*vuSDInt) > vuMin) { vu1Min=vuAv-(2*vuSDInt); } else { vu1Min=vuMin;}
            break;
        case 1:
            if (vuAv+(2*vuSDInt) < vuPeak) { vu2Peak=vuAv+(2*vuSDInt); } else { vu2Peak=vuPeak;}
            if (vuAv-(2*vuSDInt) > vuMin) { vu2Min=vuAv-(2*vuSDInt); } else { vu2Min=vuMin;}
            break;
        case 2:
            if (vuAv+(2*vuSDInt) < vuPeak) { vu3Peak=vuAv+(2*vuSDInt); } else { vu3Peak=vuPeak;}
            if (vuAv-(2*vuSDInt) > vuMin) { vu3Min=vuAv-(2*vuSDInt); } else { vu3Min=vuMin;}
            break;
      }
   }
}

/*
 * This is the calibration section which setsup the defauls in the memory of the device. 
 */


bool Sequence::isCalibrated(void)
{
#ifdef SEQUENCEDEUG
  Serial.println("checking if calibrated (default=yes)");
#endif  
  if (EEPROM.read(CALCONFIRM) == SIGNATURE ) return (true);
#ifdef SEQUENCEDEUG
  Serial.println("turned out we weren't calibrated");
#endif
  return (false);
}

void Sequence::calibrate(void)
{
  
#ifdef SEQUENCEDEUG
  Serial.println("Calibrating");
#endif  
  calibrationSample(); // Take samples to get the max,min, and averages.
#ifdef SEQUENCEDEUG
  Serial.println("Calibration complete storing these values");
  Serial.print("BASS ");
  printVoltage(vu2Peak,vu2Av,vu2Min);
  Serial.print(" :BASE AV= ");
  Serial.print(vu2Av,HEX);
  Serial.print(" MID ");
  printVoltage(vu3Peak,vu3Av,vu3Min);
  Serial.print(" TOP ");
  printVoltage(vu1Peak,vu1Av,vu1Min);
  Serial.print("\n");
#endif
  int vu1avint,vu2aving,cu3aving;
  writeEPint( CALVU1MIN,vu1Min);
  writeEPint( CALVU1AV,vu1Av);
  writeEPint( CALVU1PEAK,vu1Peak);
  writeEPint( CALVU2MIN,vu2Min);
  writeEPint( CALVU2AV,vu2Av);
  writeEPint( CALVU2PEAK,vu2Peak);
  writeEPint( CALVU3MIN,vu3Min);
  writeEPint( CALVU3AV,vu3Av);
  writeEPint( CALVU3PEAK,vu3Peak);
  writeEPint(CALCONFIRM,SIGNATURE); //Write the current value of the shaft encoder into the EEPROM for reboot.
  loadCalibrations();
}

void Sequence::writeEPint(int addr, int inp)
{
  byte LSB=inp;
  byte MSB=inp>>8;
  //EEPROM.update(addr,LSB);
  //EEPROM.update(addr+1,MSB);
}

int Sequence::readEPint(int addr)
{
  byte LSB=EEPROM.read(addr);
  byte MSB=EEPROM.read(addr+1);
  int OP=MSB;
  OP = (OP<<8);
  OP = OP|LSB;
  return OP;
}

void Sequence::loadCalibrations(void)
{

  if (isCalibrated() ==true){
    calVU1min = readEPint( CALVU1MIN);
    calVU1av = readEPint( CALVU1AV);
    calVU1peak = readEPint( CALVU1PEAK);
    calVU2min= readEPint( CALVU2MIN);
    calVU2av = readEPint( CALVU2AV);
    calVU2peak = readEPint( CALVU2PEAK);
    calVU3min = readEPint( CALVU3MIN);
    calVU3av = readEPint( CALVU3AV);
    calVU3peak = readEPint( CALVU3PEAK);
  } else {
    calVU1min=calVU2min=calVU3min=0;
    calVU1peak=calVU2peak=calVU3peak=512;
    calVU1av=calVU2av=calVU3av=255;
  }

  #ifdef SEQUENCEDEUG
  Serial.println("Calibration loadeded following values");
  Serial.print("BASS ");
  printVoltage(calVU2peak,calVU2av,calVU2min);
    Serial.print(" :BASE AV= ");
  Serial.print(calVU2av,HEX);
  Serial.print(" MID ");
  printVoltage(calVU3peak,calVU3av,calVU3min);
  Serial.print(" TREB ");
  printVoltage(calVU1peak,calVU1av,calVU1min);
  Serial.print("\n");
#endif
}

//***************************END CALBRATION SECTION ************************************


////// Simple Test Sequences ////

void Sequence::goDark()
{
  ds->offAll();
}

void Sequence::showBass()
{
  sample(0);
  int vu=map(vu2Av,calVU2min,calVU2peak,0,NUMPIXELS);
  barGraph(vu,255,0,0);
#ifdef SEQUENCEDEUG
  Serial.print("Base : Sample ");
  printVoltage(vu2Peak,vu2Av,vu2Min);
  Serial.print(" ++  : Calibration ");
  printVoltage(calVU2peak,calVU2av,calVU2min);
  Serial.print(" Scale Max ");
  Serial.print(NUMPIXELS);
  Serial.print(" Scale Result ");
  Serial.println(vu);
#endif
}

void Sequence::showMid()
{
  sample(0);
  int vu=map(vu3Av,calVU3min,calVU3peak,0,NUMPIXELS);
  barGraph(vu,0,255,0);
#ifdef SEQUENCEDEUG
  Serial.print("Mid : Sample ");
  printVoltage(vu3Peak,vu3Av,vu3Min);
  Serial.print("++  : Calibration ");
  printVoltage(calVU3peak,calVU3av,calVU3min);
  Serial.print(" Scale Max ");
  Serial.print(NUMPIXELS);
  Serial.print(" Scale Result ");
  Serial.println(vu);
#endif
}

void Sequence::showTreble()
{
  sample(0);
  int vu=map(vu1Av,calVU1min,calVU1peak,0,NUMPIXELS);
  barGraph(vu,0,0,255);
#ifdef SEQUENCEDEUG
  Serial.print("Treble : Sample ");
  printVoltage(vu1Peak,vu1Av,vu3Min);
  Serial.print(" ++ Calibration ");
  printVoltage(calVU1peak,calVU1av,calVU1min);
  Serial.print(" Scale Max ");
  Serial.print(NUMPIXELS);
  Serial.print(" Scale Result ");
  Serial.println(vu);
#endif
  
}

////// END SIMPLE EXERCISES





void Sequence::strobeWhite(int loops)
{
  for (int a = 0; a<loops;a++)
     {
      if (checkBoot()) return; //abort if interrupt happened.
       for (int a = 0;a <ds->pixels();a++)
       {
        ds->turnOn(a,255,255,255,255);
       }
      ds->show();
      delay(10);
      ds->offAll();
      ds->show();
      delay(40);
     }
}

void Sequence::strobeColour(int code,int loops)
{
  for (int count=0;count<loops;count++)
      {
        if (checkBoot()) return; //abort if interrupt happened.
        for (int a=0;a<ds->pixels();a++)
        {
          switch (code)
          {
  
            case 0: ds->turnRed(a,255);break;
            case 1: ds->turnGreen(a,255);break;
            default: ds->turnBlue(a,255); break;
          }
        }
        ds->show();
        delay(10);
        ds->offAll();
        ds->show();
        delay(40); 
      }
}


void Sequence::redBuild(void)
{
   int kitLength=10; //numper of pixels to chase each other.
    for (int wizloop=0;wizloop<ds->pixels();wizloop++)
    {
      kitLength=wizloop;
      for (int kit = 0; kit < ds->pixels() -kitLength;kit++)
      {
        if (checkBoot()) return; //abort if interrupt happened.
        ds->offAll();
        for (int lightup = 0; lightup < kitLength; lightup++)
        {
          ds->turnRed(kit+lightup,64);
        }
        ds->turnRed(kit+(kitLength/2),255);
        ds->show();
      }
  
      for (int kit = ds->pixels() -kitLength; kit >=0;kit--)
      {
        if (checkBoot()) return; //abort if interrupt happened.
        ds->offAll();
        for (int lightup = 0; lightup < kitLength; lightup++)
        {
          ds->turnRed(kit+lightup,128);
        }
        ds->turnRed(kit+(kitLength/2),255);
        ds->show();
        
      }
    } //Wizloop
}

bool Sequence::checkBoot(void)
{
  return shaftRebootFlag;
}


void Sequence::communicate(int mode)
{
  Serial.print("Selected Mode : ");
  Serial.println(mode);
  if (mode<=0)
  {
    //strobeColour(0,2);
    for (int a=0;a<20;a++){
      digitalWrite(COMMLED,HIGH);
      delay(5000);
      digitalWrite(COMMLED,LOW);
      delay(5000);
    }
  }
  else
  {
    for (int a=0;a<mode;a++){
      //strobeWhite(2);
      digitalWrite(COMMLED,HIGH);
      delay(10000);
      digitalWrite(COMMLED,LOW);
      delay(20000);
    }
  }
  delay(1000);
}

void Sequence::superFlash(void)
{  
     redBuild();
     strobeColour(1,20);
     strobeColour(2,20);
     strobeWhite(20);
     int d=200;
     for (int a=0;a<20;a ++)
     {
      strobeWhite(1);
      delay(d);
      d+=50;
     }
     strobeColour(0,5);
     if (checkBoot()) return; //abort if interrupt happened.
}

void Sequence::boogie(void)
{
    int vu1 = analogRead(TrebleAnalogPin);
    int vu2 = analogRead(BassAnalogPin);
    int vu3 = analogRead(MidAnalogPin);
    bool bassGate,midGate,topGate=false;
    if (vu1Peak<vu1) vu1Peak=vu1;
    if (vu2Peak<vu2) vu2Peak=vu2;
    if (vu3Peak<vu3) vu3Peak=vu3;
    if (vu1Min>vu1) vu1Min=vu1;
    if (vu2Min>vu2) vu2Min=vu2;
    if (vu3Min>vu3) vu3Min=vu3;

if (vu1<SEQUENCEGATEtop)
{
    topGate=true;
}
if (vu2<SEQUENCEGATEbass)
{
  bassGate=true;
}
if (vu3<SEQUENCEGATEmid)
{
  midGate=true;
}

/*
if ( (midGate + bassGate + topGate) > 0) 
{
  #ifdef SEQUENCEDEUG
  Serial.println("Gated return");
  #endif
  return;
}
*/


    
    int scale1 = vu1*ENDSCALER;
    int scale2 = vu2*MIDSCALER;
    int mid1 = (NUMPIXELS/2) + scale2 -(MIDSCALER*2);
    int mid2 = (NUMPIXELS/2) - scale2 +(MIDSCALER*2);
    int topscale = NUMPIXELS - scale2;
    int a;
  
    intensity = BASEINTENSITY+(scale1*2);
    
    for ( a=0;a<scale1;a++) {
      ds->turnGreen(a,intensity);
      ds->turnGreen(NUMPIXELS-a,scale1*10);
    }
  
    intensity=BASEINTENSITY+(scale2*10);
    for (a=0;a<scale2;a++)
    {
      ds->turnRed(mid1+a,intensity);
      ds->turnRed(mid2-a,intensity);
    }
  
    
    intensity = scale1 * scale2;
    
    for (a=0;a<NUMPIXELS;a++)
    {
      ds->turnBlue(a,intensity);
    }
    
    ds->setGlobalBrightness(INTESITYSCALER*vu3);
    ds->show();
    delay(20);
    ds->offAll();
    ds->show();
}




void Sequence::groovy(void)
{
    int a =0;
    int b=0;
    int c=10;
    int trail=30;
    int loop=1;
    for (c=0;c<trail;c++)
    {
      ds->offAll();
      for(a=0;a<NUMPIXELS;a++)
      {
        if (checkBoot()==true) return; //abort if interrupt happened.
        ds->turnOn(a+trail,RED,GREEN,BLUE,31);
        ds->turnOn(NUMPIXELS-a-trail,RED,GREEN,BLUE,31);
        for (b=0;b<c;b++)
        {
          ds->turnOn(a+trail,RED,GREEN,BLUE,1);
          ds->turnOn(NUMPIXELS-b,RED,GREEN,BLUE,1);
          if (loop==1) {if (RED++==0) if (GREEN++==0) BLUE++; }//if (BLUE=254) loop++;}
          if (loop==2) {if (GREEN++==0) if (BLUE++==0) RED++; }//if (RED=254) loop++;}
          if (loop==3) {if (BLUE++==0) if (RED++==0) GREEN++;}//if (GREEN=254) loop++;}
          if (loop==4) {if (BLUE++==0) if (GREEN++==0) RED++; }//if (RED=254) loop++;}
          if (loop==5) {if (GREEN++==0) if (RED++==0) BLUE++; }//if (BLUE=254) loop=1;}
        }
        ds->offOne(a-c+1);
        ds->offOne(NUMPIXELS+1-a);
        if (loop==6) {
          loop=0;
        }
        loop++;
      }
    }
}

void Sequence::barGraph(int level,int red, int green, int blue)
{

  ds->offAll();
  for (int a=0;a<=level;a++)
  {
    ds->turnOn(a,red,green,blue,31);
  }
  ds->show();
}

void Sequence::showAllChannels()
{
  ds->offAll();
  sample(0);
  int vu1=map(vu1Av,calVU2min,calVU2peak,0,NUMPIXELS);
  int vu2=map(vu2Av,calVU2min,calVU2peak,0,NUMPIXELS);
  int vu3=map(vu3Av,calVU2min,calVU2peak,0,NUMPIXELS);
 
  for (int a=0;a<=NUMPIXELS;a++)
  {
    int r,g,b=0;
    if (vu2>=a) r=255;
    if (vu3>=a) g=255;
    if (vu1>=a) b=255;
    ds->turnOn(a,r,g,b,31);
    if ( (a>vu1) & (a>vu2) & (a>vu3)) break;
  }
  ds->show();
}



/* 
 *  Test Routines that are used to show and calibrate the analogue signals to enable in-time with the music displays to build.
 */


void Sequence::printVoltage(int maxV, int avV, int minV)
{
  if (maxV<10) Serial.print("0");
  if (maxV<100) Serial.print("0");
  Serial.print(maxV);
  Serial.print(" : ");
  if (avV<10) Serial.print("0");
  if (avV<100) Serial.print("0");
  Serial.print(avV);
  Serial.print(" : ");
  if (minV<10) Serial.print("0");
  if (minV<100) Serial.print("0");
  Serial.print(minV);
  Serial.print(" : ");
  int PP=maxV-minV;
  
  Serial.print(" PP=");
  Serial.print(PP);

}

void Sequence::showCalibratedVolts()
{
  showVolts();
  int vu1=map(vu1Av,calVU1min,calVU1peak,0,10);
  int vu2=map(vu2Av,calVU2min,calVU2peak,0,10);
  int vu3=map(vu3Av,calVU3min,calVU3peak,0,10);
  Serial.print("BASS ");
  Serial.print(vu1);
  Serial.print(" MID ");
  Serial.print(vu3);
  Serial.print(" TREB ");
  Serial.print(vu1);
  Serial.print("\n");
}

void Sequence::showVolts(void)
{
  sample(0);
  Serial.print("BASS ");
  printVoltage(vu2Peak,vu2Av,vu2Min);
  Serial.print(" MID ");
  printVoltage(vu3Peak,vu3Av,vu3Min);
  Serial.print(" TREB ");
  printVoltage(vu1Peak,vu1Av,vu1Min);
  Serial.print("\n");
}




