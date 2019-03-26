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
    Serial.println("reset counter to min");
  }
  if (shaftCounter>sMax){
    shaftCounter=sMax;
    Serial.println("reset counter to max");
  }
}

//This is a reboot function that can be used to reset the unit.
void shaftReboot() {
  wdt_disable();
  wdt_enable(WDTO_15MS);
  while (1) {}
}


//This flashes the designated pin (built in LED for example) the number of times recorded by the shaft encoder
void shaftConfirm(int flashCount)
{
#ifdef DEBUGSHAFTENCODER
      Serial.print("Flashing the Light ");
      Serial.println(flashCount);
 #endif
  for (int a = 0;  a<=flashCount; a++)
  {
    digitalWrite(LED_BUILTIN,HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN,LOW);
    delay(250);
  }
#ifdef DEBUGSHAFTENCODER
      Serial.println("Finished Flashing");
#endif
}

//When the switch is pushed, this will
void shaftPushSwitchISR()
{
  noInterrupts();
  #ifdef DEBUGSHAFTENCODER
      Serial.println("Button Push Detected");
      delay(500);
   #endif
   int debounceDelay=350; // This was found by trial and error to be the amount of time needed to debounce the switch
  bool state,prevState;

  //This is a debounce routine, stops the repeated calls to the interrupt.
  for (int looper=0;looper < debounceDelay;looper++)
  {
    delay(1);
    state=digitalRead(shaftPushSw);
    if (state != prevState)
    {
      looper=0;
      prevState=state;
    }
  }
  //We now have a stable switch which is not bouncing all over the place
    
  EEPROM.write(shaftPROPOSEDRUNSTATE,shaftCounter); //Write the current value of the shaft encoder into the EEPROM for reboot.
  //Perform Watchdog Reboot
  shaftReboot();
}

void shaftRotateISR()
{
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
         shaftCounter ++;
       } else {
         shaftCounter --;
       }
     
     }
   }
   checkShaftCounter();
        
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
