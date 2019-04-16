#ifndef ENCODERH
#define ENCODERH

#include <EEPROM.h> //Library needed to read and write from the EEPROM
#include <avr/wdt.h> //Library needed to use the watchdog timer - which is used in the reboot method

#define DEBUGSHAFTENCODER

//These are the digital pins that the shaft encoder will be connected to.
#define shaftPushSw 2
#define shaftOutputA 3
#define shaftOutputB 4
#define hwResetControl 5
#define DEBOUNCEDELAY 350; // This was found by trial and error to be the amount of time needed to debounce the switch
#define LONGPRESSDELAY 750; //Time in microseconds for a long press to be registered.

#ifndef EEPROMGLOBALS

//These are used to address EEPROM memory.
#define SIGNATURE 0xAB
#define shaftCURRENTRUNSTATE 1
#define shaftPROPOSEDRUNSTATE 2
#define shaftCONFIRMLED LED_BUILTIN


#endif


//These are persistent variables used to assess the rotation of the shaftencoder, and keep a tally of where we are

bool shaftAState;
bool shaftALastState;
bool shaftBState;
bool shaftBLastState;

int shaftCounter;
int sMax,sMin;
bool shaftRebootFlag=false;
bool shaftLongPressFlag=false;

#endif
