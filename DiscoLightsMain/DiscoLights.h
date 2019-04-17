#define DEBUGMAIN

#ifndef EEPROMGLOBALS
#define EEPROMGLOBALS
#define SIGNATURE 0xAB // This is to see if it's a virgin board or not.

//These are used to address EEPROM memory.
//Shaft Encoder
#define shaftCURRENTRUNSTATE 1
#define shaftPROPOSEDRUNSTATE 2
#define shaftCONFIRMLED 9 // the digital output pin connected to the panel LED.
#define shaftMAX 15

//For VU Calibration.
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

