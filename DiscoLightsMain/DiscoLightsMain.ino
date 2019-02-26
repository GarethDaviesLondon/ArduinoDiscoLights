
#define NUMPIXELS 144 // Number of LEDs in strip
#define DATAPIN    4
#define CLOCKPIN   5
#define BARGRAPHMAX 144

int bargraph;
  
void setup() {
  pinMode(DATAPIN, OUTPUT);
  pinMode(CLOCKPIN,OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  wipe();

  /*
  OCR0A=0xAF;
  TIMSK0 |= _BV(OCIE0A);
  */

}



void loop() {
  lightgraph2(BARGRAPHMAX);
  while (bargraph > 0)
  {
    lightgraph(bargraph);
    bargraph--;
  }
}

int interuptNum=0;

SIGNAL (TIMER0_COMPA_vect)
{
  interuptNum++;
  if (interuptNum < 100)
  {
    digitalWrite(LED_BUILTIN,HIGH);
  }
  else
  {
    if (interuptNum <200)
    {
      digitalWrite(LED_BUILTIN,LOW);
    }
    else
    {
      interuptNum=0;
    }
  }
}



void lightgraph2(int bLevel)
{
  bargraph=bLevel;
  StartTX();
  for (int a=0;a<NUMPIXELS-bLevel;a++)
  {
    clearPixel();
  }
  for (int a=bLevel;a>0;a--)
  {
    commandPixel(1,0,0);
  }
  EndTX();
}

void lightgraph(int bLevel)
{
  bargraph=bLevel;
  StartTX();
  for (int a=0;a<NUMPIXELS-bLevel;a++)
  {
    clearPixel();
  }
  for (int a=bLevel;a>0;a--)
  {
    commandPixel(1,0,0);
  }
  EndTX();
}



void sendBits(bool val)
//Data is clocked out on the rising clock edge.
{
  //noInterrupts();
  digitalWrite(CLOCKPIN,LOW);
  digitalWrite(DATAPIN,val);
  digitalWrite(CLOCKPIN,HIGH);
  //interrupts();
}

void sendBit(bool val)
//Data is clocked out on the rising clock edge.
{
  PORTD=B00000000 | (val << 4);
  PORTD=B00100000 | (val << 4);

  /*
  //noInterrupts();
  digitalWrite(CLOCKPIN,LOW);
  digitalWrite(DATAPIN,val);
  digitalWrite(CLOCKPIN,HIGH);
  //interrupts();
  */
}



void StartTX()
{
  for (int a=0; a<32; a++) sendBit(LOW);
}

void EndTX()
{
  for (int a=0; a<NUMPIXELS; a++) sendBit(LOW);
}

void wipe()
{
  StartTX();
  for (int a=0;a<NUMPIXELS;a++)
  {
    clearPixel();
  }
  EndTX();
}

void clearPixel()
{
    for (int b=0;b<3;b++) sendBit(HIGH);
    for (int b=0;b<29;b++) sendBit(LOW);
}

void commandPixel(unsigned int red,unsigned int green, unsigned int blue)
{
  //send GlobalContrl 
  sendByteHigh();
  if (red>0) {sendByteHigh();}else{sendByteLow();}
  if (green>0) {sendByteHigh();}else{sendByteLow();}
  if (blue>0) {sendByteHigh();}else{sendByteLow();} 
}

void sendByteHigh()
{
  for (int a=0; a<8; a++)
  {
    sendBit(HIGH);
  }
}

void sendByteLow()
{
  for (int a=0; a<8;a++)
  {
    sendBit(LOW);
  }
}


void turnon(int Pixel, int Red, int Green, int Blue)
{
  StartTX();
  for (int a =0;a<Pixel;a++) commandPixel(0,0,0);
  commandPixel(Red,Green,Blue);
  for (int b=Pixel+1;b<NUMPIXELS;b++) commandPixel(0,0,0);
  EndTX();
}

