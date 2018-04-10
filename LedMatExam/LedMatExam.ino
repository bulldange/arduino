#include <LEDMatrix.h>
uint8_t LedCLKpin     = 11;
uint8_t LedCSpin      = 12;
uint8_t LedDINpin     = 13;

/*
 * LEDMatrix
 */
LEDMatrix Led;

void setup() {
  // put your setup code here, to run once:

  Led.Init_MAX7219(LedCLKpin,LedCSpin,LedDINpin);
}

void loop() {
  // put your main code here, to run repeatedly:
  for ( int i=0;i<10;i++)
    for ( int k=0;k<10;k++)
    {
          Led.DisplayOnLed(i,k);
          delay(500);
    }  
}
