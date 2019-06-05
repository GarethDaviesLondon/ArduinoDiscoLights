/****
 * 
 * This code has a fudge to remove the bass channel
 * this is now set to be the mid-range (equal)
 * 
 */

#include "Sequence.h"
#include "ShaftEncoder.h"

//#define SEQUENCEDEUG
//#define SEQUENCEDEUGSAMPLE
//#define SEQUENCEDEBUGCALIBRATE
//#define SHOWCALIBVALS

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
  flashtrigger=10; //this is used in the mixItUp routine
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
    
    float vu1amp=0.8;
    float vu2amp=1.2;
    float vu3amp=1.2;
    
    pinSequence[0]=TrebleAnalogPin;
    pinSequence[1]=TrebleAnalogPin;
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

    //Apply some gain to normalise the channels
    vu1Sam=vu1Sam*vu1amp;
    vu2Sam=vu2Sam*vu2amp;
    vu3Sam=vu3Sam*vu3amp;
    
      float scale;
      scale = 
        (vu1Sam-vu1Min)
        *ds->pixels()
        /
        (vu1Peak-vu1Min)
;
        
      vu1Scale=scale;

      
       scale = 
        (vu2Sam-vu2Min)
        *ds->pixels()
        /
        (vu2Peak-vu2Min)
        ;
        
      //scale = ((vu2Sam-vu2Min)/(vu2Peak-vu2Min))*ds->pixels();
      vu2Scale=scale;
      //scale = ((vu3Sam-vu3Min)/(vu3Peak-vu3Min))*ds->pixels();
      
       scale = 
        (vu3Sam-vu3Min)
        *ds->pixels()
        /
        (vu3Peak-vu2Min)
        ;
        
      vu3Scale=scale;

/**********************
 * 
 * THIS IS THE FUDGE REMOVES THE BASE SAMPLE
 * 
 */
      //CANCEL THE BASE BASS SCALE
      vu2Scale=vu3Scale;
      
      //vu1Scale=map(vu1Sam,v12Min,vu1Peak,0,ds->pixels());
      //vu2Scale=map(vu2Sam,vu2Min,vu2Peak,0,ds->pixels());
      //vu3Scale=map(vu3Sam,vu3Min,vu3Peak,0,ds->pixels());
      
      if (vu1Scale>ds->pixels()) {vu1Scale=ds->pixels();} 
      if (vu2Scale>ds->pixels()) {vu2Scale=ds->pixels();}
      if (vu3Scale>ds->pixels()) {vu3Scale=ds->pixels();}

      if (vu1Scale<0) {vu1Scale=0;} 
      if (vu2Scale<0) {vu2Scale=0;}
      if (vu3Scale<0) {vu3Scale=0;}
  
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
     #endif
     #ifdef SHOWCALIBVALS
        Serial.print("Results 1,2,3 ");
        Serial.print(vu1Sam);
        Serial.print(" scaled ");
        Serial.print(vu1Scale);
        Serial.print(":");
        Serial.print(vu1Min);
        Serial.print(",");
        Serial.print(vu1Peak);
        Serial.print("++");
        Serial.print(vu2Sam);
        Serial.print(" scaled ");
        Serial.print(vu2Scale);
        Serial.print(":");
        Serial.print(vu2Min);
        Serial.print(",");
        Serial.print(vu2Peak);
        Serial.print("++");
        Serial.print(vu3Sam);
        Serial.print(" scaled ");
        Serial.print(vu3Scale);
        Serial.print(":");
        Serial.print(vu3Min);
        Serial.print(",");
        Serial.print(vu3Peak);
        Serial.print("++");
        Serial.println();
      #endif 
}
      
//Routine for capturing a calibration sample and calculating mean and std deviation.
//Sets the limits for light triggering based on 2 StdDev from mean as vu[1,2,3]Min and Vu[1,2,3]Peak

void Sequence::calibrationSample()
{
    for (int inputPin=0;inputPin<3;inputPin++)
    {
      calibrationSample(inputPin);
    }
}


void Sequence::calibrationSample(int inputPin)
{

    int sampleset = CALIBRATIONWINDOW;
    int starttime,stoptime,endtime;
    int pinSequence[3];
    pinSequence[0]=TrebleAnalogPin;
    pinSequence[1]=BassAnalogPin;
    pinSequence[2]=MidAnalogPin;
    
    int vuSamples [sampleset];
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
              if (inputPin==1) delay(1); //reduce sampling rate for the base
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
          bool vufudge= false;
      
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
          if (vuSDInt<10) 
          {
            vuSDInt=10; //This provides some beat behaviour even if calibrated in silence, otherwise it's always on!
            vufudge=true;
          }
      
      #ifdef SEQUENCEDEBUGCALIBRATE
        endtime=millis();
        Serial.print("Processing stoptime ");
        Serial.print(endtime);
        elapsedMillis=endtime-starttime;
        Serial.print(" Elapsed milli sceconds ");
        Serial.println(elapsedMillis);
       #endif
       #ifdef SHOWCALIBVALS
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
            if (vuAv+(2*vuSDInt) < vuPeak) { vu1Peak=vuAv+(1*vuSDInt); } else { vu1Peak=vuPeak;}
            if (vuAv-(2*vuSDInt) > vuMin) { vu1Min=vuAv-(1*vuSDInt); } else { vu1Min=vuMin;}
            if (vufudge==true) {vu1Min=vuMin;}
            vu1Av=vuAv;
            break;
        case 1: //needs more sensitivity here as the dynamic range is rubbish on the base
            if (vuAv+(1*vuSDInt) < vuPeak) { vu2Peak=vuAv+(1*vuSDInt); } else { vu3Peak=vuPeak;}
            if (vuAv-(1*vuSDInt) > vuMin) { vu2Min=vuAv-(1*vuSDInt); } else { vu3Min=vuMin;}
            vu2Av=vuAv;
            if (vufudge==true) {vu2Min=vuMin;}
            break;
        case 2:
            if (vuAv+(1*vuSDInt) < vuPeak) { vu3Peak=vuAv+(1*vuSDInt); } else { vu3Peak=vuPeak;}
            if (vuAv-(1*vuSDInt) > vuMin) { vu3Min=vuAv-(1*vuSDInt); } else { vu3Min=vuMin;}
            if (vufudge==true) {vu3Min=vuMin;}
            vu3Av=vuAv;
            break;
       }
       
       /*
               switch (inputPin)
       {
        case 0:
            vu1Peak=vuPeak;
            vu1Min=vuMin;
            break;
        case 1:
            vu2Peak=vuPeak;
            vu2Min=vuMin;
            break;
        case 2:
            vu3Peak=vuPeak;
            vu3Min=vuMin;

            break;
       }
       */
       
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



/////////////////////////////////////////
//level driven patterns (graphs) here
////////////////////////////////////////

void Sequence::barGraph(int level,int red, int green, int blue)
{

  ds->offAll();
  for (int a=0;a<=level;a++)
  {
    ds->turnOn(a,red,green,blue,31);
  }
  ds->show();
}

void Sequence::inverseBarGraph(int level,int red, int green, int blue)
{

  ds->offAll();
  for (int a=ds->pixels();a>=ds->pixels()-level;a--)
  {
    ds->turnOn(a,red,green,blue,31);
  }
  ds->show();
}

void Sequence::midBarGraph(int level,int red, int green, int blue)
{

  ds->offAll();
  for (int a=ds->pixels()/2;a>=(ds->pixels()/2)-(level/2);a--)
  {
    ds->turnOn(a,red,green,blue,31);
  }
  ds->show();
  for (int a=ds->pixels()/2;a<=(ds->pixels()/2)+(level/2);a++)
  {
    ds->turnOn(a,red,green,blue,31);
  }
  ds->show();
}


void Sequence::randomColour(int level,int col)
{
  barGraph(level,col,255-col,255-(255-col));
}

void Sequence::inverseRandomColour(int level, int col)
{
  inverseBarGraph(level,col,255-col,255-(255+col));
}

void Sequence::midRandomColour(int level, int col)
{
  midBarGraph(level,col,255-col,255-(255+col));
}

void Sequence::rainbow(int level)
{

  if (level>=ds->pixels()-5) {level=ds->pixels()-5;}
  ds->offAll();
  for (int a=0;a<level;a+=5)
  {
    ds->turnOn(a,255,0,0,31);
    ds->turnOn(a+1,255,255,0);
    ds->turnOn(a+2,0,255,0);
    ds->turnOn(a+3,0,0,255);
    ds->turnOn(a+4,143,0,255);
  }
  ds->show();
}

void Sequence::inverseRainbow(int level)
{
 if (level>=ds->pixels()-5) {level=ds->pixels()-5;}
 ds->offAll();
  for (int a=ds->pixels();a>(ds->pixels()-(level/2));a-=5)
  {
    ds->turnOn(a,255,0,0,31);
    ds->turnOn(a-1,255,255,0);
    ds->turnOn(a-2,0,255,0);
    ds->turnOn(a-3,0,0,255);
    ds->turnOn(a-4,143,0,255);
  }
  ds->show(); 
}

void Sequence::midRainbow(int level)
{
 ds->offAll();
 if (level>=ds->pixels()-5) {level=ds->pixels()-5;}
 int mid=ds->pixels()/2;
 int sig=level/2;
 //Go left;
  for (int a=mid;a>(mid-(sig));a-=5)
  {
    ds->turnOn(a,255,0,0,31);
    ds->turnOn(a-1,255,255,0);
    ds->turnOn(a-2,0,255,0);
    ds->turnOn(a-3,0,0,255);
    ds->turnOn(a-4,143,0,255);
  }
   //Go right;
  for (int a=mid;a<(mid+(sig));a+=5)
  {
    ds->turnOn(a,255,0,0,31);
    ds->turnOn(a+1,255,255,0);
    ds->turnOn(a+2,0,255,0);
    ds->turnOn(a+3,0,0,255);
    ds->turnOn(a+4,143,0,255);
  }
  ds->show(); 
}

///////////////////Non measurement routines

void Sequence::strobeWhite(int loops)
{
  for (int a = 0; a<loops;a++)
     {
      if (shaftInterruptOccurred==true) return NULL;
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
        if (shaftInterruptOccurred==true) return NULL;
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
      if (shaftInterruptOccurred==true) return NULL;
      kitLength=wizloop;
      for (int kit = 0; kit < ds->pixels()-kitLength;kit++)
      {
        ds->offAll();
        for (int lightup = 0; lightup < kitLength; lightup++)
        {
          ds->turnRed(kit+lightup,64);
        }
        ds->turnRed(kit+(kitLength/2),255);
        if (shaftInterruptOccurred==true) return NULL;
        ds->show();
      }
  
      for (int kit = ds->pixels() -kitLength; kit >=0;kit--)
      {
        ds->offAll();
        for (int lightup = 0; lightup < kitLength; lightup++)
        {
          ds->turnRed(kit+lightup,128);
        }
        ds->turnRed(kit+(kitLength/2),255);
        if (shaftInterruptOccurred==true) return NULL;
        ds->show();
      }
    } //Wizloop
}


void Sequence::superFlash(int rep)
{  
    for (int x=0;x<rep;x++)
    {
       if (shaftInterruptOccurred==true) return NULL;
       strobeWhite(5);
       if (shaftInterruptOccurred==true) return NULL;
       strobeColour(0,5);
       if (shaftInterruptOccurred==true) return NULL;
       strobeColour(1,5);
       if (shaftInterruptOccurred==true) return NULL;
       strobeColour(2,5);
       if (shaftInterruptOccurred==true) return NULL;
    }
}

void Sequence::lightsRight(int pix,int red)
{
      ds->offAll();
      int green = 255-(red/2);
      int blue = green;
      int maxP=ds->pixels();
      if (pix<5) {pix=6;}
      if (pix> maxP){pix=maxP-6;}
      ds->turnOn(pix+6,red,green,blue,31);
      for (int a=pix+5;a>pix;a--) //this gives the flash-fade to the left of the main
      {
        ds->turnOn(a,red,green,blue,5);
        delay(10);
        ds->show();
      }
      //ds->show();
}

void Sequence::lightsLeft(int pix,int red)
{
      ds->offAll();
      int green = 255-(red/2);
      int blue = green;
      int maxP=ds->pixels();
      if (pix<5) {pix=6;}
      if (pix> maxP){pix=maxP-6;}
      ds->turnOn(maxP-pix-6,red,green,blue,31);
      for (int a=(maxP-pix-5);a<(maxP-pix);a++)
      {
        ds->turnOn(a,red,green,blue,5);
        delay(10);
        ds->show();
      }
      //ds->show();
}

void Sequence::lightsMid(int pix,int red)
{
      ds->offAll();
      int maxP=ds->pixels();
      if (pix<6) {pix=6;}
      if (pix> maxP){pix=maxP-6;}
      maxP=maxP/2;
      pix=pix/2;
      int green = 255-(red/2);
      int blue = green;

      //from Left trail to the middle
      ds->turnOn(maxP-pix-3,red,green,blue,31); //left from middle
      for (int a=maxP-pix-2;a<=(maxP-pix);a++)
      {
        ds->turnOn(a,red,green,blue,5);
        ds->show();
      }
      
      //from right end to the middle
      ds->turnOn(maxP+pix+3,red,green,blue,31); //right from middle
      for (int a=maxP+pix+2;a>=(maxP+pix);a--)
      {
        ds->turnOn(a,red,green,blue,5);
        ds->show();
      }
}


///Simple Patterns to music


void Sequence::showBass(){showBass(0);}
void Sequence::showBass(int mode)
{
  sample();
  switch (mode)
  {
    case 0:
        barGraph(vu2Scale,255,0,0);
        break;
    case 1:
        inverseBarGraph(vu2Scale,255,0,0);
        break;
    default:
        midBarGraph(vu2Scale,255,0,0);
        break;
  }

#ifdef SEQUENCEDEUG
  Serial.print("Base : Sample ");
  Serial.print(vu2Sam);
  Serial.print(" ++  : Calibration ");
  printVoltage(vu2Peak,vu2Av,vu2Min);
  Serial.print(" Scale Max ");
  Serial.print(ds->pixels());
  Serial.print(" Scale Result ");
  Serial.println(vu);
#endif
}

void Sequence::showMid() {showMid(0);}
void Sequence::showMid(int mode)
{
  sample();
  switch (mode)
  {
    case 0:
        barGraph(vu3Scale,0,255,0);
        break;
    case 1:
        inverseBarGraph(vu3Scale,0,255,0);
        break;
    default:
        midBarGraph(vu3Scale,0,255,0);
        break;
  }

#ifdef SEQUENCEDEUG
  Serial.print("Base : Sample ");
  Serial.print(vu3Sam);
  Serial.print(" ++  : Calibration ");
  printVoltage(vu3Peak,vu3Av,vu3Min);
  Serial.print(" Scale Max ");
  Serial.print(ds->pixels());
  Serial.print(" Scale Result ");
  Serial.println(vu);
#endif
}

void Sequence::showTreble() {showTreble(0);}
void Sequence::showTreble(int mode)
{
  sample();
  switch (mode)
  {
    case 0:
        barGraph(vu1Scale,0,0,255);
        break;
    case 1:
        inverseBarGraph(vu1Scale,0,0,255);
        break;
    default:
        midBarGraph(vu1Scale,0,0,255);
        break;
  }
#ifdef SEQUENCEDEUG
  Serial.print("Base : Sample ");
  Serial.print(vu1Sam);
  Serial.print(" ++  : Calibration ");
  printVoltage(vu1Peak,vu1Av,vu1Min);
  Serial.print(" Scale Max ");
  Serial.print(ds->pixels());
  Serial.print(" Scale Result ");
  Serial.println(vu);
#endif
  
}


void Sequence::channelMovesRed()
{
  sample();
  lightsMid(vu2Scale,random(0,50));
  lightsRight(vu1Scale,random(0,100));
  lightsLeft(vu3Scale,random(150,250));
}

void Sequence::colourMix()
{
  sample();
  int r=vu2Scale+vu2Scale+vu1Scale+vu3Scale;
  r=map(r,ds->pixels()*4,0,255,0);
  int g=255-r;
  int b=255-r-g;
  barGraph(vu1Scale,r,g,b);
  inverseBarGraph(vu2Scale,r,g,b);
}


void Sequence::rainbowMix()
{
  sample();
  rainbow(vu1Scale);
  delay(10);
  inverseRainbow(vu3Scale);
  delay(10);
  midRainbow(vu2Scale);
}

void Sequence::randomMix()
{
  sample();
  randomColour(vu1Scale,random(100,200));
  midRandomColour(vu2Scale,random(0,255));
  inverseRandomColour(vu3Scale,random(0,100));
}


void Sequence::mixItUp()
{
 int a=random(0,10);
 int mode=random(0,2);
 int repcount=random(15,75);
 sample();
 
  switch(a)
  {
    case 1:
          for (int z=0;z<repcount/4;z++)
          {
            channelMovesRed();
          }
          break;
    case 2:
     for (int z=0;z<repcount;z++)
          {
            colourMix();
          }
          break;
    case 3:
     for (int z=0;z<repcount;z++)
          {
            rainbowMix();
          }
          break;
    case 4:
     for (int z=0;z<repcount;z++)
          {
            randomMix();
          }
          break;
    case 5:
     for (int z=0;z<repcount;z++)
          {
            showTreble(mode);
          }
          break;
    case 6:
     for (int z=0;z<repcount;z++)
          {
            showBass(mode);
          }
          break;
    case 7:
     for (int z=0;z<repcount;z++)
          {
            showMid(mode);
          }
          break;
    case 8:
      
          break;
    case 9:
     
          break;
    case 10:
           flashtrigger++;
           if (flashtrigger==10) 
           {
            flashtrigger=0;
            superFlash(1);
           }
    default:
          break;
  }
}





