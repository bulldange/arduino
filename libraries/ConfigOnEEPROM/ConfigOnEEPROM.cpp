

#include "ConfigOnEEPROM.h"
//#define _DEBUG_

void ConfigOnEEPROM::readIntDataFromEEPROM(int address,int *data,char len){  
  for (int i=0;i<(int)len;i++)
  {
     *(data+i)=readIntFromEEPROM(address+i*sizeof(int));
#ifdef _DEBUG_  
    Serial.println("Read IntData , address,i,data " + String(address+i*sizeof(int)) + " "+String(i) + " "+ String(*(data+i) ));
#endif       
  }
}
void ConfigOnEEPROM::writeIntDataIntoEEPROM(int *address,int *data,char len){  
#ifdef _DEBUG_  
    Serial.println("***writeIntDataIntoEEPROM , address,len " + String(*address) + " "+ String((int)len) );
#endif  
  for (int i=0;i<(int)len;i++)
  {
     writeIntToEEPROM(*address,*(data+i));
     #ifdef _DEBUG_  
     Serial.println("writeIntDataIntoEEPROM , address,i,data " + String(*address,DEC) + " "+String(i) + " "+ String(*(data+i) ));
     #endif 
     *address+=sizeof(int);
  }
}

void ConfigOnEEPROM::writeHeaderIntoEEPROM(int address,char header,char len){  // ���� : CHAR+SIZE(1Byte) 
  EEPROM.write(address,(byte)header);
  EEPROM.write(address+1,(byte)len);
}

char ConfigOnEEPROM::readHeaderFromEEPROM(int address,char *len){  // ���� : CHAR+SIZE(1Byte) 

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

// ����Ÿ�� �а��� ���� ����Ÿ ������ �����Ѵ�. 0�� �ɶ����� ���Լ��� �ݺ��Ͽ� ȣ�� 
// ����Ÿ �����ΰ�� HEADER_ERR�� ���� 
int ConfigOnEEPROM::readinitFromEEPROM(int *address,int * data,char *len){
  // ����б�
  char HEADER;
  

  if (*address == 0){
  	  HEADER=readHeaderFromEEPROM(*address,len);
		#ifdef _DEBUG_  
		  Serial.println("readinitFromEEPROM " + String(HEADER) + " "+String((int)*len) );
		#endif  
	  if ( HEADER == 'S'){    // ���� ��� 
	  	Cnt = (int)*len;
		*address+=HEADER_SIZE;	
  
	  }
	  else
	  {
	  	return HEADER_ERR;
	  }  	
  }

	 HEADER=readHeaderFromEEPROM(*address,len);
	 *address+=HEADER_SIZE;
	#ifdef _DEBUG_  
	  Serial.println("readinitFromEEPROM " + String(HEADER) + " "+String((int)*len) );
	#endif  
	
  switch(HEADER){
  	case 'I':  // integer 
	    readIntDataFromEEPROM(*address,data,*len);
	      *address+=*len*sizeof(int); 
	    break;
    default :
      return HEADER_ERR; 
  }
  #ifdef _DEBUG_  
  Serial.println("readinitFromEEPROM CNT=" + String(Cnt)  );
  #endif  
  Cnt--;
  return Cnt;
}

void ConfigOnEEPROM::writeInitIntoEEPROM(int *address,int* data,char len){
   // ����б�
  char HEADER;
#ifdef _DEBUG_  
  Serial.println("writeInitIntoEEPROM (addr)" + String(*address,DEC) );
#endif 
  if (*address == 0){   // ������� �Է� 
  	Cnt = 0;
	writeHeaderIntoEEPROM(*address,'S',Cnt);  	  
	*address+=HEADER_SIZE;	
  }
   
  writeHeaderIntoEEPROM(*address,'I',len);
  *address+=HEADER_SIZE;
  writeIntDataIntoEEPROM(address,data,len);
  Cnt++;
  EEPROM.write(1,Cnt);  // ����Ÿ ���� ��� 
}




bool ConfigOnEEPROM::writeVoidAt(int address,void* value,int len){
  byte* pValue = (byte*)value;
  byte byteValue;
  
  if (address < 0 || len <= 0)
     return false;
 
// Serial.println("writeVoidAt pValue=" + String(*((unsigned long*)pValue)));
     
  EEPROM.write(address,'V');  // HEADER
  address ++;
  for(int i=0;i<len;i++){
    byteValue=*(pValue+i);
    EEPROM.write(address+i,byteValue);
   // Serial.println("writeVoidAt pValue=" + String(address+i));
   }
  
  return true;
}

bool ConfigOnEEPROM::readVoidAt(int address,void* value,int len){
  byte* pValue = (byte*)value;
  byte byteValue;
 
  if (address < 0 || len <= 0)
     return false;
	  
  char header=EEPROM.read(address);
  if (header != 'V')
     return false;
     
  address++;   
  for(int i=0;i<len;i++){
    *(pValue+i)=EEPROM.read(address+i);
///    Serial.println("readVoidAt address=" + String(address+i));
 
  }

 //  Serial.println("readVoidAt pValue=" + String(*((unsigned long*)pValue)));
 
  return true;
}

