#include "IRCommand.h"

IRCommand::IRCommand(){
	oldTm=millis();
}
char IRCommand::getIR(IRrecv irrecv,unsigned int *delayTm){
  char r;   
  decode_results  results;        // Somewhere to store the results
  if (irrecv.decode(&results)) {    // Grab an IR code
    irrecv.resume();
    switch(results.value)
    {
    	case IR_POWER  :
    		r = 'W';
    		break;
        case IR_TEST   :
		    r =  'T'; 
    		break;
        case IR_BACK   :
        	r =  'B';
    		break;		 
        case IR_MENU   :
        	r =  'M';
		    break; 
        case IR_REW    :
			r =  'R'; 
            break;
        case IR_FF     :
        	r =  'F';
		    break; 
        case IR_PLAY   :
			r =  'P'; 
            break;
        case IR_PLUS   :
			r =  '+'; 
            break;
        case IR_MINUS  :
			r =  '-'; 
            break;
        case IR_CLEAR  :
			r =  'C'; 
            break;
        case IR_ZERO   :
			r =  '0'; 
            break;
        case IR_ONE    :
			r =  '1'; 
            break;
        case IR_TWO    : 
        	r =  '2';
            break;
        case IR_THREE  :
			r =  '3'; 
            break;
        case IR_FOUR   :
			return '4'; 
            break;
        case IR_FIVE   :
			return '5'; 
            break;
        case IR_SIX    :
			return '6'; 
            break;
        case IR_SEVEN  :
			r =  '7'; 
            break;
        case IR_EIGHT  :
			r =  '8'; 
            break;
        case IR_NINE   :
			r =  '9'; 
            break;
        default:
           return 'N';
	}
	unsigned long currTm=millis();
    *delayTm = currTm-oldTm;
	oldTm=currTm; 
    return r;            // 데이타를 수신중.       
  }
  return 'N'; 	
  
}
