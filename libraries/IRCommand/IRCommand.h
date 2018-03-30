#ifndef _IR_COMMAND_
#define _IR_COMMAND_

#include "Arduino.h"
#include <IRremote.h>

#define IR_POWER  0xFFA25D
#define IR_TEST   0xFF22DD 
#define IR_BACK   0xFFC23D 
#define IR_MENU   0xFFE21D 
#define IR_REW    0xFFE01F 
#define IR_FF     0xFF906F 
#define IR_PLAY   0xFFA857 
#define IR_PLUS   0xFF02FD 
#define IR_MINUS  0xFF9867 
#define IR_CLEAR  0xFFB04F 
#define IR_ZERO   0xFF6897 
#define IR_ONE    0xFF30CF 
#define IR_TWO    0xFF18E7 
#define IR_THREE  0xFF7A85 
#define IR_FOUR   0xFF10EF 
#define IR_FIVE   0xFF38C7 
#define IR_SIX    0xFF5AA5 
#define IR_SEVEN  0xFF42BD 
#define IR_EIGHT  0xFF4AB5 
#define IR_NINE   0xFF52AD 


class IRCommand{
	private:
	   unsigned long oldTm;  // 바로전에 입력되었던 시간. 
    public : 
      char getIR(IRrecv irrecv,unsigned int *delayTm);
      IRCommand(); 

};

#endif
