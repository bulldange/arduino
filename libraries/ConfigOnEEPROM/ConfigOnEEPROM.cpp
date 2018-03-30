

#include "ConfigOnEEPROM.h"

void ConfigOnEEPROM::readIntDataFromEEPROM(int address,int *data,char len){  
#ifdef _DEBUG_  
    Serial.println("***Read IntData , address,len " + String(address) + " "+ String((int)len) );
#endif       
  for (int i=0;i<(int)len;i++)
  {
     *(data+i)=readIntFromEEPROM(address+i*sizeof(int));
#ifdef _DEBUG_  
    Serial.println("Read IntData , address,i,data " + String(address+i*sizeof(int)) + " "+String(i) + " "+ String(*(data+i) ));
#endif       
  }
}
void ConfigOnEEPROM::writeIntDataIntoEEPROM(int address,int *data,char len){  
#ifdef _DEBUG_  
    Serial.println("***writeIntDataIntoEEPROM , address,len " + String(address) + " "+ String((int)len) );
#endif  
  for (int i=0;i<(int)len;i++)
  {
     writeIntToEEPROM(address+i*sizeof(int),*(data+i));
     #ifdef _DEBUG_  
    Serial.println("writeIntDataIntoEEPROM , address,i,data " + String(address+i*sizeof(int)) + " "+String(i) + " "+ String(*(data+i) ));
    #endif 
  }
}

void ConfigOnEEPROM::writeHeaderIntoEEPROM(int address,char header,char len){  // 구성 : CHAR+SIZE(1Byte) 
  EEPROM.write(address,header);
  EEPROM.write(address+1,(byte)len);
}

char ConfigOnEEPROM::readHeaderFromEEPROM(int address,char *len){  // 구성 : CHAR+SIZE(1Byte) 

  char header = EEPROM.read(address);
  *len        = EEPROM.read(address+1); 
#ifdef _DEBUG_  
    Serial.println("Read Header is  "+ String(header) + ","+String(int(*len)));
#endif    
  return header;
}

void ConfigOnEEPROM::writeIntToEEPROM(int address,int value){
  byte* pValue = (byte*)&value;
  byte byteValue;

  for(int i=0;i<sizeof(int);i++){
    byteValue=*(pValue+i);
    EEPROM.write(address+i,byteValue);
  }
}

int ConfigOnEEPROM::readIntFromEEPROM(int address){
  int value;
  byte* pValue = (byte*)&value;
  byte byteValue;

  for(int i=0;i<sizeof(int);i++){
    *(pValue+i)=EEPROM.read(address+i);
  }
  return value;
}

// 데이타를 읽고나서 남은 데이타 갯수를 리턴한다. 0이 될때까지 이함수를 반복하여 호출 
// 데이타 오류인경우 HEADER_ERR을 리턴 
int ConfigOnEEPROM::readinitFromEEPROM(int *address,int * data,char *len){
  // 헤더읽기
  char HEADER;
  
  HEADER=readHeaderFromEEPROM(*address,len);

  if (*address == 0){
	  if ( HEADER == 'S'){    // 시작 헤더 
	  	Cnt = len;	  
		*address+=HEADER_SIZE;	
    	HEADER=readHeaderFromEEPROM(*address,len);
	  }
	  else
	  {
	  	return HEADER_ERR;
	  }  	
  }
   
  *address+=HEADER_SIZE;

#ifdef _DEBUG_  
  Serial.println("readinitFromEEPROM " + String(HEADER) + " "+String((int)*len) );
#endif  

   
  switch(HEADER){
  	case 'I':  // integer 
	    readIntDataFromEEPROM(*address,data,*len);
	    break;
    default :
      return HEADER_ERR; 
  }
  
  Cnt--;
  *address+=*len; 
  return Cnt;
}

void ConfigOnEEPROM::writeInitIntoEEPROM(int address,int* data,char len){
   // 헤더읽기
  char HEADER;

  if (address == 0){   // 시작헤더 입력 
	  if ( HEADER == 'S'){    // 시작 헤더 
	  	Cnt = 0;
		writeHeaderIntoEEPROM(address,'S',Cnt);  	  
		address+=HEADER_SIZE;	
	  }
	  else
	  {
	  	return HEADER_ERR;
	  }  	
  }
   
  writeHeaderIntoEEPROM(address,'I',len);
  address+=HEADER_SIZE;
  writeIntDataIntoEEPROM(address,data,len);
  Cnt++;
  EEPROM.write(1,Cnt);  // 데이타 갯수 기록 
}

