#include <LEDMatrix.h>

LEDMatrix L;
void setup() {
  // put your setup code here, to run once:
  L.Init_MAX7219(4,3,2);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  L.DisplayOnLed(random(10),random(10));
  L.DispProgressBar(random(9));
  delay(500);
}
