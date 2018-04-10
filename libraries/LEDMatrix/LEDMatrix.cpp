#include "LEDMatrix.h"
    
void LEDMatrix::DisplayOnLed(int d1,int d2)
{
   for(int i=1;i<7;i++)
   { 
     if ( i==6)
     { 
        if (DotBlink) 
            Dot=0x08;
        else
            Dot=0x00;    
         DotBlink = !DotBlink;    
     }
     else
        Dot=0x00;
       
     Write_Max7219(i,(SmallNum6[d1][i-1] << 4) | SmallNum6[d2][i-1] | Dot );      
   }
}

void LEDMatrix::DispProgressBar(int Pos)
{
  Write_Max7219(8,ProgressBar[Pos]);            
}


void LEDMatrix::Write_Max7219_byte(unsigned char DATA) 
{   
  digitalWrite(Max7219_pinCS,LOW);    
  for(unsigned char i=8;i>=1;i--)
  {     
     digitalWrite(Max7219_pinCLK,LOW);
     digitalWrite(Max7219_pinDIN,DATA&0x80);// Extracting a bit data
     DATA = DATA<<1;
     digitalWrite(Max7219_pinCLK,HIGH);
   }                                 
}    
 
void LEDMatrix::Write_Max7219(unsigned char address,unsigned char dat)
{
  digitalWrite(Max7219_pinCS,LOW);
  Write_Max7219_byte(address);           //address，code of LED
  Write_Max7219_byte(dat);               //data，figure on LED 
  digitalWrite(Max7219_pinCS,HIGH);
}
 
void LEDMatrix::Init_MAX7219(int pinCLK,int pinCS,int pinDIN)
{
	
 Max7219_pinCLK = pinCLK;
 Max7219_pinCS  = pinCS;
 Max7219_pinDIN = pinDIN;
  	
 DotBlink=true;
 pinMode(Max7219_pinCLK,OUTPUT);
 pinMode(Max7219_pinCS,OUTPUT);
 pinMode(Max7219_pinDIN,OUTPUT);
 delay(50);
    	
 Write_Max7219(0x09, 0x00);       //decoding ：BCD
 Write_Max7219(0x0a, 0x03);       //brightness 
 Write_Max7219(0x0b, 0x07);       //scanlimit；8 LEDs
 Write_Max7219(0x0c, 0x01);       //power-down mode：0，normal mode：1
 Write_Max7219(0x0f, 0x00);       //test display：1；EOT，display：0
for(uint8_t i=1;i<9;i++)
  Write_Max7219(i,ProgressBar[0]);
}


  



