#include "Sequence.h"
#include "ShaftEncoder.h"

#define SEQUENCEDEUG

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

#ifdef SEQUENCEDEUG
debugCounter++;
if (debugCounter > 25)
{
  debugCounter=0;
  Serial.print("BASS ");
  if (vu2Peak<10) Serial.print("0");
  if (vu2Peak<100) Serial.print("0");
  Serial.print(vu2Peak);
  Serial.print (":");
  if (vu2Min<10) Serial.print("0");
  if (vu2Min<100) Serial.print("0");
  Serial.print(vu2Min);
  Serial.print(" MID ");
  if (vu3Peak<10) Serial.print("0");
  if (vu3Peak<100) Serial.print("0");
  Serial.print(vu3Peak);
  Serial.print (":");
  if (vu3Min<10) Serial.print("0");
  if (vu3Min<100) Serial.print("0");
  Serial.print(vu3Min);
  Serial.print(" TOP ");
  if (vu1Peak<10) Serial.print("0");
  if (vu1Peak<100) Serial.print("0");
  Serial.print(vu1Peak);
  Serial.print (":");
  if (vu1Min<10) Serial.print("0");
  if (vu1Min<100) Serial.print("0");
  Serial.print(vu1Min);
  Serial.print("\n");
}
#endif
    
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
        if (checkBoot()) return; //abort if interrupt happened.
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

