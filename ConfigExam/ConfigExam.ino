#include <ConfigOnEEPROM.h>

#define _DEBUG_

int  Sig[3][100]    = {{1,2,3,4,5},{6,7,8},{9,10,11,12}};
char SigLen[3]      = {5,3,4};    // 시그날 크기
int  SigCnt         = 3;      // 시그날 갯수
int  AddrEEPROM     = 0;      // EEOROM 주소
char len =0;
ConfigOnEEPROM Config;
  char h;

void setup() {
  Serial.begin(9600);

  // put your setup code here, to run once:
 /* for ( int i=0;i<SigCnt;i++) {
    Config.writeInitIntoEEPROM(&AddrEEPROM,Sig[i],SigLen[i]);  // Header   
    Serial.println("AddrEEPROM=" + String(AddrEEPROM,DEC));    
  }*/

   int address=0;
    int r;int i=0;
    do {
      r=Config.readinitFromEEPROM(&address,Sig[i],&SigLen[i]);      
      i++;
      Serial.println("R=" + String(r,DEC));    

    }while(r>0);

  
  //h=Config.readHeaderFromEEPROM(AddrEEPROM,&len);
 // Serial.println("Sig,SigLen=" + String(AddrEEPROM)+","+String(len,DEC)+","+h);
}

void loop() {
  
}
