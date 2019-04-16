#include "Sequence.h"
#include "ShaftEncoder.h"

//#define SEQUENCEDEUG
//#define SEQUENCEDEUGSAMPLE
//#define SEQUENCEDEBUGCALIBRATE

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
  loadCalibrations();
}

//Some methods for controlling flow
bool Sequence::checkBoot(void)
{
  return shaftRebootFlag;
}


/*
 * This is the main sample capture routine.
 * During calibration it averages a larger number of samples
 * The debug flag SEQUENCEDEUGSAMPLE turns on a lot of reporting
 * this makes it very slow, so only use when debugging!
 */

//Grabs a sample of data and calculates the peak value experienced in the window, this is used as the vu1,vu2,vu3 measurements of current sample.

void Sequence::sample()
{
    int sampleset = SAMPLEWINDOW;
    int starttime,stoptime,endtime;
    int pinSequence[3];
    pinSequence[0]=TrebleAnalogPin;
    pinSequence[1]=BassAnalogPin;
    pinSequence[2]=MidAnalogPin;

    int vuSample;
    vu1Sam=vu2Sam=vu3Sam=0;


      #ifdef SEQUENCEDEUGSAMPLE
      Serial.println("Sampling Now");
      Serial.print(" Sample Window Size per channel: ");
      Serial.println(sampleset);
      starttime=millis();
      Serial.print("Starttime ");
      Serial.println(starttime);
      #endif 

    //Loop taking a sample and then doing a peak-detection.  
    for (int a = 0; a < sampleset; a++)
    {
              vuSample = analogRead(TrebleAnalogPin);
              if (vuSample>vu1Sam) vu1Sam=vuSample;
              vuSample = analogRead(BassAnalogPin);
              if (vuSample>vu2Sam) vu2Sam=vuSample;
              vuSample = analogRead(MidAnalogPin);
              if (vuSample>vu3Sam) vu3Sam=vuSample;;
    }

      vu1Scale=map(vu1Sam,vu1Min,vu1Peak,0,NUMPIXELS);
      vu2Scale=map(vu2Sam,vu2Min,vu2Peak,0,NUMPIXELS);
      vu3Scale=map(vu3Sam,vu3Min,vu3Peak,0,NUMPIXELS);

      #ifdef SEQUENCEDEUGSAMPLE
        stoptime=millis();
        Serial.print("Sampling stoptime ");
        Serial.println(stoptime);
        int elapsedMillis=stoptime-starttime;
        Serial.print("Elapsed Sampling Time ");
        Serial.println(elapsedMillis);
        float millisPerSample=((float) elapsedMillis/(float) sampleset)*3; // 3x because there are three channels sampling.
        Serial.print("Milli Seconds Per Sample ");
        Serial.println(millisPerSample);
        float sampFreq=1000/millisPerSample;
        Serial.print("Sampling Frequency (Hz)");
        Serial.println(sampFreq);
        Serial.print("Channel Samples 1,2,3 ");
        Serial.print(vu1Sam);
        Serial.print(", ");
        Serial.print(vu2Sam);
        Serial.print(", ");
        Serial.print(vu3Sam);
        Serial.println();
      #endif 
}
      
//Routine for capturing a calibration sample and calculating mean and std deviation.
//Sets the limits for light triggering based on 2 StdDev from mean as vu[1,2,3]Min and Vu[1,2,3]Peak

void Sequence::calibrationSample()
{

    int sampleset = CALIBRATIONWINDOW;
    int starttime,stoptime,endtime;
    int pinSequence[3];
    pinSequence[0]=TrebleAnalogPin;
    pinSequence[1]=BassAnalogPin;
    pinSequence[2]=MidAnalogPin;
    
    int vuSamples [sampleset];

    for (int inputPin=0;inputPin<3;inputPin++)
    {
      #ifdef SEQUENCEDEBUGCALIBRATE
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
      
      #ifdef SEQUENCEDEBUGCALIBRATE
        stoptime=millis();
        Serial.print("Sampling stoptime ");
        Serial.println(stoptime);
        int elapsedMillis=stoptime-starttime;
        Serial.print("Elapsed Sampling Time ");
        Serial.println(elapsedMillis);
        float millisPerSample=((float) elapsedMillis/(float) sampleset);
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
          int vuSDInt=(int)vuSD;      //It rounds down the SD to an integer, faster and less storage requirements.
          if (vuSDInt<10) vuSDInt=10; //This provides some beat behaviour even if calibrated in silence, otherwise it's always on!
      
      #ifdef SEQUENCEDEBUGCALIBRATE
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
            vu1Av=vuAv;
            break;
        case 1:
            if (vuAv+(2*vuSDInt) < vuPeak) { vu2Peak=vuAv+(2*vuSDInt); } else { vu2Peak=vuPeak;}
            if (vuAv-(2*vuSDInt) > vuMin) { vu2Min=vuAv-(2*vuSDInt); } else { vu2Min=vuMin;}
            vu2Av=vuAv;
            break;
        case 2:
            if (vuAv+(2*vuSDInt) < vuPeak) { vu3Peak=vuAv+(2*vuSDInt); } else { vu3Peak=vuPeak;}
            if (vuAv-(2*vuSDInt) > vuMin) { vu3Min=vuAv-(2*vuSDInt); } else { vu3Min=vuMin;}
            vu3Av=vuAv;
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
  EEPROM.update(addr,LSB);
  EEPROM.update(addr+1,MSB);
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
    vu1Min = readEPint( CALVU1MIN);
    vu1Av = readEPint( CALVU1AV);
    vu1Peak = readEPint( CALVU1PEAK);
    vu2Min = readEPint( CALVU2MIN);
    vu2Av = readEPint( CALVU2AV);
    vu2Peak = readEPint( CALVU2PEAK);
    vu3Min = readEPint( CALVU3MIN);
    vu3Av = readEPint( CALVU3AV);
    vu3Peak = readEPint( CALVU3PEAK);
  } else {
    vu1Min=vu1Min=vu1Min=0;
    vu1Av=vu2Av=vu3Av=255;
    vu1Peak=vu2Peak=vu3Peak=512;
  }

  #ifdef SEQUENCEDEUG
  Serial.println("Calibration loadeded following values");
  Serial.print("BASS ");
  printVoltage(vu2Peak,vu2Av,vu2Min);
  Serial.print(" MID ");
  printVoltage(vu3Peak,vu3Av,vu3Min);
  Serial.print(" TREB ");
  printVoltage(vu1Peak,vu1Av,vu1Min);
  Serial.print("\n");
#endif
}

//***************************END CALBRATION SECTION ************************************


////// Simple Test Sequences ////

void Sequence::goDark()
{
  ds->offAll();
  ds->show();
}

void Sequence::showBass()
{
  sample();
  barGraph(vu2Scale,255,0,0);
#ifdef SEQUENCEDEUG
  Serial.print("Base : Sample ");
  Serial.print(vu2Sam);
  Serial.print(" ++  : Calibration ");
  printVoltage(vu2Peak,vu2Av,vu2Min);
  Serial.print(" Scale Max ");
  Serial.print(NUMPIXELS);
  Serial.print(" Scale Result ");
  Serial.println(vu);
#endif
}

void Sequence::showMid()
{
  sample();
  barGraph(vu3Scale,0,255,0);
#ifdef SEQUENCEDEUG
  Serial.print("Base : Sample ");
  Serial.print(vu3Sam);
  Serial.print(" ++  : Calibration ");
  printVoltage(vu3Peak,vu3Av,vu3Min);
  Serial.print(" Scale Max ");
  Serial.print(NUMPIXELS);
  Serial.print(" Scale Result ");
  Serial.println(vu);
#endif
}

void Sequence::showTreble()
{
  sample();
  barGraph(vu1Scale,0,0,255);
#ifdef SEQUENCEDEUG
  Serial.print("Base : Sample ");
  Serial.print(vu1Sam);
  Serial.print(" ++  : Calibration ");
  printVoltage(vu1Peak,vu1Av,vu1Min);
  Serial.print(" Scale Max ");
  Serial.print(NUMPIXELS);
  Serial.print(" Scale Result ");
  Serial.println(vu);
#endif
  
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

////// END SIMPLE EXERCISES


//Fixed Pattern Sequences here


void Sequence::barGraph(int level,int red, int green, int blue)
{

  ds->offAll();
  for (int a=0;a<=level;a++)
  {
    ds->turnOn(a,red,green,blue,31);
  }
  ds->show();
}

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


//Multi Channel Effects


void Sequence::boogie(void)
{
    sample();
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


void Sequence::showAllChannels()
{
  ds->offAll();
  sample();
  int vu1=map(vu1Sam,vu1Min,vu1Peak,0,NUMPIXELS);
  int vu2=map(vu2Sam,vu2Min,vu2Peak,0,NUMPIXELS);
  int vu3=map(vu3Sam,vu2Min,vu2Peak,0,NUMPIXELS);
 
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


