
#include <ConfigOnEEPROM.h>

ConfigOnEEPROM Config;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println(String(sizeof(int)) + "," + String(sizeof(long)));
  int address=1000;
  unsigned long a=1232431;
  int b=4567;
 // Serial.println(String(a));
  int len=sizeof(unsigned long);
 if( Config.writeVoidAt(address,&a,sizeof(unsigned long)) )
    Serial.println("Write ");
 if( Config.writeVoidAt(address+5,&b,sizeof(int) ))
    Serial.println("Write ");   

 if ( Config.readVoidAt(address,(void*)&a,sizeof(unsigned long)) ) 
  Serial.println(String(a));
 if ( Config.readVoidAt(address+5,(void*)&b,sizeof(int)) ) 
  Serial.println(String(b));

}

void loop() {
  // put your main code here, to run repeatedly:

}
