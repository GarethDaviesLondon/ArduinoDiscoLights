#ifndef ENCODERH
#define ENCODERH

#include <EEPROM.h> //Library needed to read and write from the EEPROM
#include <avr/wdt.h> //Library needed to use the watchdog timer - which is used in the reboot method

#define DEBUGSHAFTENCODER

//These are the digital pins that the shaft encoder will be connected to.
#define shaftPushSw 2
#define shaftOutputA 3
#define shaftOutputB 4


//These are used to address EEPROM memory.
#define SIGNATURE 0xAB
#define shaftCURRENTRUNSTATE 1
#define shaftPROPOSEDRUNSTATE 2
#define shaftCONFIRMLED LED_BUILTIN

//These are persistent variables used to assess the rotation of the shaftencoder, and keep a tally of where we are

bool shaftAState;
bool shaftALastState;
bool shaftBState;
bool shaftBLastState;

int shaftCounter;
int sMax,sMin;

#endif
