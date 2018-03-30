#ifndef _CONFIG_ON_EEPROM_
#define _CONFIG_ON_EEPROM_
#include <EEPROM.h>
#include "Arduino.h"

#define HEADER_SIZE 2
#define HEADER_ERR  -1

class ConfigOnEEPROM
{   
	private:
		uint8_t Cnt; // item ����  
	
		void readIntDataFromEEPROM(int address,int *data,char len);
		void writeIntDataIntoEEPROM(int address,int *data,char len);
		void writeHeaderIntoEEPROM(int address,char header,char len);
		char readHeaderFromEEPROM(int address,char *len);
		void writeIntToEEPROM(int address,int value);
		int readIntFromEEPROM(int address);
	public:
		int readinitFromEEPROM(int *address,int * data,char *len);	
	    void writeInitIntoEEPROM(int address,int* data,char len);
};

#endif
