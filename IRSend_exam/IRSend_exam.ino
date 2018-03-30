#include <IRremote.h>

IRsend irsend;

void setup()
{
   
}

void loop() {
  int khz = 38; // 38kHz carrier frequency for the NEC protocol
  unsigned int  irSignal[67] = {8700,4500, 500,600, 500,600, 550,1650, 500,650, 500,600, 450,650, 500,600, 500,600, 500,1700, 500,1750, 500,550, 550,1700, 550,1700, 450,1750, 550,1650, 500,1750, 550,550, 500,600, 550,550, 500,1750, 500,600, 500,600, 500,600, 500,600, 550,1650, 500,1750, 500,1700, 500,600, 550,1700, 500,1700, 550,1700, 450,1750, 550};
  irsend.sendRaw(irSignal, sizeof(irSignal) / sizeof(irSignal[0]), khz); //Note the approach used to automatically calculate the size of the array.

  delay(15000); //In this example, the signal will be repeated every 5 seconds, approximately.
}
