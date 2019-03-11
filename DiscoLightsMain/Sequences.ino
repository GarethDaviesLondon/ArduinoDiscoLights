#include "Sequences.h"

Sequence::Sequence (DotStrip *dotin)
{
  ds=dotin;
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
      //stopTimerInterrupt();
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
      Update = true;
      //startTimerInterrupt();
      if (loop==6) {
        loop=0;
      }
      loop++;
    }
  }
}

