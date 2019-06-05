void initShaftEncoder(int Min,int Max)
{
  
  //Set up the end-stops of the shaftencoder;
  sMin=Min;
  sMax=Max;

  shaftAState = shaftALastState = shaftBState = shaftBLastState = false;
  //pinMode(shaftCONFIRMLED, OUTPUT);
  pinMode (LED_BUILTIN,OUTPUT);
  pinMode(shaftOutputA,INPUT_PULLUP);
  pinMode(shaftOutputB,INPUT_PULLUP);
  pinMode(shaftPushSw,INPUT_PULLUP);
  pinMode(hwResetControl,INPUT_PULLUP);
  
  shaftBState = digitalRead(shaftOutputB);
  shaftAState = digitalRead(shaftOutputA); // Reads the "current" state of the outputA
  shaftALastState = shaftAState; // Updates the previous state of the outputA with the current state
  shaftBLastState = shaftBState;
  
  if (EEPROM.read(0) != SIGNATURE)
    {
#ifdef DEBUGSHAFTENCODER
      Serial.println("Initialising first run of new shaft encoder");
#endif
      EEPROM.write(shaftCURRENTRUNSTATE,0);
      EEPROM.write(shaftPROPOSEDRUNSTATE,0);
      EEPROM.write(0,SIGNATURE);
    }
    else
    {
#ifdef DEBUGSHAFTENCODER
      Serial.println("I'm not a shaft encoding virgin, signature found");
#endif
    }
    shaftCounter=EEPROM.read(shaftPROPOSEDRUNSTATE);
    checkShaftCounter();
    
    attachInterrupt(digitalPinToInterrupt(shaftOutputA),shaftRotateISR,CHANGE);
    attachInterrupt(digitalPinToInterrupt(shaftPushSw),shaftPushSwitchISR,LOW);

}

//Just makes sure that the counter is setup right.
void checkShaftCounter()
{
#ifdef DEBUGSHAFTENCODER
      Serial.print("Checking the Max/Min ");
      Serial.print(sMax);
      Serial.print("/");
      Serial.println(sMin);
      Serial.print("Value to check ");
      Serial.println(shaftCounter);
#endif
  //Incase the end-stops are the wrong way around.
  if (sMin>sMax) {
#ifdef DEBUGSHAFTENCODER
      Serial.println("Oops that's reversed ");
#endif   
    int tmp=sMin;
    sMin=sMax;
    sMax=tmp;
#ifdef DEBUGSHAFTENCODER
      Serial.print("That's better Max/Min ");
      Serial.print(sMax);
      Serial.print("/");
      Serial.println(sMin);
#endif
  }

  //Incase that we have end-stopped.
  if (shaftCounter<sMin) {
    shaftCounter=sMin;
    #ifdef DEBUGSHAFTENCODER
    Serial.println("reset counter to min");
    #endif
  }
  if (shaftCounter>sMax){
    shaftCounter=sMax;
    #ifdef DEBUGSHAFTENCODER
    Serial.println("reset counter to max");
    #endif
  }
}

//This is a reboot function that can be used to reset the unit.
void shaftReboot() {
  shaftRebootFlag=true;
  shaftCounter=EEPROM.read(shaftPROPOSEDRUNSTATE);
}

void shaftLongPress(){
  shaftLongPressFlag=true;
}

float wasteTime(long int d)
{
    d=d*100000000;
#ifdef DEBUGSHAFTENCODER
      Serial.println(micros());
      Serial.print("Starting to waste time ");
      Serial.println(d);
 #endif
  double a=3.14;
  for (int l=0;l<d;l++)
  {
    double b=sqrt(a);
    a+=b;
    a=sq(a);
    a=a-1;
    a=sqrt(a);
  }
 #ifdef DEBUGSHAFTENCODER
      Serial.println(micros());
      Serial.println("done");
 #endif
}

//This flashes the designated pin (built in LED for example) the number of times recorded by the shaft encoder
void shaftConfirm(int flashCount)
{
#ifdef DEBUGSHAFTENCODER
      Serial.print("Flashing the Light ");
      Serial.println(flashCount);
 #endif
  unsigned long timerRecord,timerNow;
  float rubbish;
  for (int a = 0;  a<=flashCount; a++)
  {
    digitalWrite(COMMLED,HIGH);
    rubbish = wasteTime(50);
    
 /*    This code just fails, no idea what is going  on with the micros() function!
     timerRecord=timerNow=micros();
 #ifdef DEBUGSHAFTENCODER
      Serial.print("Light On starting Wait at: ");
      Serial.println(timerRecord);
 #endif
    while(timerNow < timerRecord+500) {
      timerNow=micros();
#ifdef DEBUGSHAFTENCODER
      Serial.print("Still Waiting at : ");
      Serial.print(timerNow);
      Serial.print("Waiting to exceed ");
      Serial.println(timerRecord+500);
 #endif
     }//Delay funtion, the delay and millis don't work during interrupts
     */
     
    digitalWrite(COMMLED,LOW);
    rubbish = wasteTime(50);
    
  /* This micros timer code just doesn't work!
    timerRecord=timerNow=micros();
 #ifdef DEBUGSHAFTENCODER
      Serial.print("Light Off starting Wait at: ");
      Serial.println(timerRecord);
 #endif
    while(timerNow < timerRecord+500) {
      timerNow=micros();
#ifdef DEBUGSHAFTENCODER
      Serial.print("Still Waiting at : ");
      Serial.print(timerNow);
      Serial.print("Waiting to exceed ");
      Serial.println(timerRecord+500);
 #endif
     }//Delay funtion, the delay and millis don't work during interrupts
 */

 
  }
  if (rubbish>0) return NULL;
#ifdef DEBUGSHAFTENCODER
      Serial.println("Finished Flashing");
#endif
}

//When the switch is pushed, this will
void shaftPushSwitchISR()
{
  shaftInterruptOccurred=true;
  noInterrupts();
  #ifdef DEBUGSHAFTENCODER
      Serial.println("Button Push Detected");
   #endif
  int debounceDelay=DEBOUNCEDELAY; // This was found by trial and error to be the amount of time needed to debounce the switch
  int longPressDelay=LONGPRESSDELAY;
  
  bool state,prevState;

  int start=micros();
  int stoptime=start;

  //This is a debounce routine, stops the repeated calls to the interrupt.
  for (int looper=0;looper < debounceDelay;looper++)
  {
    state=digitalRead(shaftPushSw);
    if (state != prevState)
    {
      looper=0;
      prevState=state;
    }
  }
  //We now have a stable switch which is not bouncing all over the place

  state=digitalRead(shaftPushSw);
  prevState=state;
  
  while (1)
  {
    state=digitalRead(shaftPushSw);
    if (prevState!=state)
    {
      stoptime=micros();
      break;
    }
  }

  #ifdef DEBUGSHAFTENCODER
  Serial.println(start);
  Serial.println(stoptime);
  #endif

  
  if ((stoptime-start)>=longPressDelay)
  {
    #ifdef DEBUGSHAFTENCODER
    Serial.print("Long Press Delay : Threshold ");
    Serial.print(stoptime-start);
    Serial.print("::");
    Serial.println(longPressDelay);
    #endif
    shaftLongPress();
  }
  else
  {
    #ifdef DEBUGSHAFTENCODER
    Serial.println("Short Press Delay : Threshold ");
    Serial.print(stoptime-start);
    Serial.print("::");
    Serial.println(longPressDelay);
    #endif
  
    EEPROM.write(shaftPROPOSEDRUNSTATE,shaftCounter); //Write the current value of the shaft encoder into the EEPROM for reboot.
    shaftReboot();
    pattern->communicate(shaftCounter);
  }
  interrupts();
}

void shaftRotateISR()
{
   shaftInterruptOccurred=true;
   noInterrupts();
   shaftBState = digitalRead(shaftOutputB);
   shaftAState = digitalRead(shaftOutputA); // Reads the "current" state of the outputA
   
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (shaftAState == false)
   {
     if (shaftAState != shaftALastState)
     {     
       // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
       if (shaftBState != shaftAState) 
       { 
         shaftCounter --;
       } else {
         shaftCounter ++;
       }
     }
   }
   checkShaftCounter();
   shaftConfirm(shaftCounter);
        
#ifdef DEBUGSHAFTENCODER
     Serial.print ("A Postition Last ");
     Serial.print (shaftALastState);
     Serial.print (" Now ");
     Serial.print (shaftAState);
     Serial.print (" B Postition Last ");
     Serial.print (shaftBLastState);
     Serial.print (" Now ");
     Serial.print (shaftBState);
     Serial.print(" Position: ");
    Serial.println(shaftCounter);
#endif

   shaftALastState = shaftAState; // Updates the previous state of the outputA with the current state
   shaftBLastState = shaftBState;
   interrupts();
}

