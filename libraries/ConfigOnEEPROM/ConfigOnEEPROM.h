#ifndef _CONFIG_ON_EEPROM_
#define _CONFIG_ON_EEPROM_
#include <EEPROM.h>
#include "Arduino.h"

#define HEADER_SIZE 2
#define HEADER_ERR  -1

class ConfigOnEEPROM
{   
	public:
		uint8_t Cnt; // item °¹¼ö  
	
		void readIntDataFromEEPROM(int address,int *data,char len);
		void writeIntDataIntoEEPROM(int *address,int *data,char len);
		void writeHeaderIntoEEPROM(int address,char header,char len);
		char readHeaderFromEEPROM(int address,char *len);
		void writeIntToEEPROM(int address,int value);
		int readIntFromEEPROM(int address);

		int readinitFromEEPROM(int *address,int * data,char *len);	
	    void writeInitIntoEEPROM(int *address,int* data,char len);
	    bool readVoidAt(int address,void* value,int size);
	    bool writeVoidAt(int address,void* value,int size);
};
#endif
