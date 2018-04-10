int dataIn = 2;
int load = 3;
int clock = 4;

int maxInUse = 4;    //change this variable to set how many MAX7219's you'll use

int e = 0;           // just a varialble

                     // define max7219 registers
byte max7219_reg_noop        = 0x00;
byte max7219_reg_digit0      = 0x01;
byte max7219_reg_digit1      = 0x02;
byte max7219_reg_digit2      = 0x03;
byte max7219_reg_digit3      = 0x04;
byte max7219_reg_digit4      = 0x05;
byte max7219_reg_digit5      = 0x06;
byte max7219_reg_digit6      = 0x07;
byte max7219_reg_digit7      = 0x08;
byte max7219_reg_decodeMode  = 0x09;
byte max7219_reg_intensity   = 0x0a;
byte max7219_reg_scanLimit   = 0x0b;
byte max7219_reg_shutdown    = 0x0c;
byte max7219_reg_displayTest = 0x0f;

void putByte(byte data) {
  byte i = 8;
  byte mask;
  while(i > 0) {
    mask = 0x01 << (i - 1);      // get bitmask
    digitalWrite( clock, LOW);   // tick
    if (data & mask){            // choose bit
      digitalWrite(dataIn, HIGH);// send 1
    }else{
      digitalWrite(dataIn, LOW); // send 0
    }
    digitalWrite(clock, HIGH);   // tock
    --i;                         // move to lesser bit
  }
}

void maxSingle( byte reg, byte col) {    
//maxSingle is the "easy"  function to use for a     //single max7219

  digitalWrite(load, LOW);       // begin     
  putByte(reg);                  // specify register
  putByte(col);//((data & 0x01) * 256) + data >> 1); // put data   
  digitalWrite(load, LOW);       // and load da shit
  digitalWrite(load,HIGH); 
}

void maxAll (byte reg, byte col) {    // initialize  all  MAX7219's in the system
  int c = 0;
  digitalWrite(load, LOW);  // begin     
  for ( c =1; c<= maxInUse; c++) {
  putByte(reg);  // specify register
  putByte(col);//((data & 0x01) * 256) + data >> 1); // put data
    }
  digitalWrite(load, LOW);
  digitalWrite(load,HIGH);
}

void maxOne(byte maxNr, byte reg, byte col) {    
//maxOne is for adressing different MAX7219's, 
//whilele having a couple of them cascaded

  int c = 0;
  digitalWrite(load, LOW);  // begin     

  for ( c = maxInUse; c > maxNr; c--) {
    putByte(0);    // means no operation
    putByte(0);    // means no operation
  }

  putByte(reg);  // specify register
  putByte(col);//((data & 0x01) * 256) + data >> 1); // put data 

  for ( c =maxNr-1; c >= 1; c--) {
    putByte(0);    // means no operation
    putByte(0);    // means no operation
  }

  digitalWrite(load, LOW); // and load da shit
  digitalWrite(load,HIGH); 
}


void setup () {


  pinMode(dataIn, OUTPUT);
  pinMode(clock,  OUTPUT);
  pinMode(load,   OUTPUT);

  Serial.begin(9600);
  digitalWrite(13, HIGH);  

//initiation of the max 7219
  maxAll(max7219_reg_scanLimit, 0x07);      
  maxAll(max7219_reg_decodeMode, 0x00);  // using an led matrix (not digits)
  maxAll(max7219_reg_shutdown, 0x01);    // not in shutdown mode
  maxAll(max7219_reg_displayTest, 0x00); // no display test
   for (e=1; e<=8; e++) {    // empty registers, turn all LEDs off 
    maxAll(e,0);
  }
  maxAll(max7219_reg_intensity, 0x0f & 0x0f);    // the first 0x0f is the value you can set
                                                  // range: 0x00 to 0x0f
}  

void loop () {


  //if you use just one MAX7219 it should look like this
  
   maxSingle(1,1);                       //  + - - - - - - -
   maxSingle(2,2);                       //  - + - - - - - -
   maxSingle(3,4);                       //  - - + - - - - -
   maxSingle(4,8);                       //  - - - + - - - -
   maxSingle(5,16);                      //  - - - - + - - -
   maxSingle(6,32);                      //  - - - - - + - -
   maxSingle(7,64);                      //  - - - - - - + -
   maxSingle(8,128);                     //  - - - - - - - +

  delay(2000);
  //if you use more than one MAX7219, it should look like this

  
  maxAll(1,1);                       //  + - - - - - - -
  maxAll(2,3);                       //  + + - - - - - -
  maxAll(3,7);                       //  + + + - - - - -
  maxAll(4,15);                      //  + + + + - - - -
  maxAll(5,31);                      //  + + + + + - - -
  maxAll(6,63);                      //  + + + + + + - -
  maxAll(7,127);                     //  + + + + + + + -
  maxAll(8,255);                     //  + + + + + + + +
    delay(2000);

  //

  //if you use more then one max7219 the second one should look like this


  maxOne(2,1,1);                       //  + - - - - - - -
  maxOne(2,2,2);                       //  - + - - - - - -
  maxOne(2,3,4);                       //  - - + - - - - -
  maxOne(2,4,8);                       //  - - - + - - - -
  maxOne(2,5,16);                      //  - - - - + - - -
  maxOne(2,6,32);                      //  - - - - - + - -
  maxOne(2,7,64);                      //  - - - - - - + -
  maxOne(2,8,128);                     //  - - - - - - - +


  // 
  delay(2000);

}

