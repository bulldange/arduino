#include <IRCommand.h>

#include <LEDMatrix.h>
#include <IRremote.h>

#include <ConfigOnEEPROM.h>
#define _DEBUG_

//#define _MEGA_
#define _UNO_

#define READY        1
#define NOT_READY    0
#define IR_RCVING    2        // 신호를 수신중

#define HEADER_SIZE  2
#define BUTTON_IR_RCV    1    // IR_Button_On
#define BUTTON_NOCHANGE  0    

#define DETECT       1
#define NOT_DETECT   0

#define DETECT_TIME_MIN 4     // 단위 초
#define DETECT_TIME_MAX 600   // 10분

#define DETECT_RAT       0.8   // 80%

#define ON  true
#define OFF false

#define DIST_UNIT 20   //20cm
#define NOT_DETECT_TIME_UNIT 10000  //10초단위
#define NOT_DETECT_TIME_MAX 300000  // 5분
#define NOT_DETECT_TIME_MIN 10000   // 10초

#define ADDR_DETECT_TIME 1000  // 감지 시간 저장 주소
#define ADDR_DISTANCE    1010   // 감지 거리 저장 주소  

#define LED_DIST_LMT  0
#define LED_DIST_TM   1

#define BLINK_DELAY 600

#define URTRA_CHK_TIME 5000  

unsigned long urtraChkOldTime=0 ;

volatile int statusOfSystem = NOT_READY ; // 0: 초기, 1:초기화 완료
volatile int buttonStatus   = BUTTON_NOCHANGE;

unsigned long NOT_DETECT_TIME = 10000;  // 10초

uint8_t LedMode             = LED_DIST_LMT;
int statusOfDetect          = NOT_DETECT;

int notDectectCnt = 0; 
int DectectCnt    = 0; 

bool OnOff = false;   // status of Power    
/*
 *  button interrupt pin
 */
#ifdef _MEGA_
uint8_t IRRcvInterruptPin = 2;  // pin 21  : 리모컨 신호 수신
uint8_t InitInterruptPin  = 20;  // Interrupt 3  : 초기화
uint8_t IRSndPin          = 9;
#endif

#ifdef _UNO_
uint8_t IRRcvInterruptPin = 0;  //* pin 2  : 리모컨 신호 수신
uint8_t InitInterruptPin  = 20;  // Interrupt 3  : 초기화
uint8_t IRSndPin          = 3;  //*
#endif

int distAnalogPin     = A0;  
uint8_t redPinOut     = 4;  //* 1초에 두번씩 깜박이는 경우는 초기화 준비, 1초에 3번 깜빡이는 경우 초기화 확인.
uint8_t greenPinOut   = 5;  //* 사람을 감지 한 경우 1초에 한번씩 점멸.
uint8_t bluePinOut    = 6;  //* 감지하지 못 하는 경우 1초에 한번씩 점멸.
uint8_t triggerPin    = 7;  //*
uint8_t echoPin       = 8;  //*
uint8_t IRRcvPin      = 10; //*
uint8_t LedCLKpin     = 11; //*
uint8_t LedCSpin      = 12; //*
uint8_t LedDINpin     = 13; //*

unsigned long redDelayTime   = -1;  
unsigned long greenDelayTime = -1;
unsigned long blueDelayTime  = -1;

unsigned long redOldTime,greenOldTime,blueOldTime;

uint8_t redLedStatus;
uint8_t greenLedStatus;
uint8_t blueLedStatus;

volatile unsigned long IRRcvTime = 0; 
int DISTANCE       =  150;  // 탐지거리
int distance;               // 실측거리

unsigned long IRoldTm = 0;

/*
 * 적외선 관련 
 */
int  Sig[3][100]    = {0};
char SigLen[3]      = {0};    // 시그날 크기
int  SigCnt         = 0;      // 시그날 갯수
int  AddrEEPROM     = 0;      // EEOROM 주소
IRrecv irrecv(IRRcvPin);
IRsend irsend;

/*
 *  초음파 관련
 */
unsigned long dectectStartTime=0;

/*
 * LEDMatrix
 */
LEDMatrix Led;

/*
*  Config On EEPROM
*  환경저장하기
*/
ConfigOnEEPROM Config;

/*
 *  Remocon Controller
 */

IRCommand irComm;
void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  
  IRrecv irrecv(IRRcvPin);
  irrecv.enableIRIn();  // Start the receiver  

  Led.Init_MAX7219(LedCLKpin,LedCSpin,LedDINpin);
  // utrasonic  
  pinMode(triggerPin,OUTPUT);
  pinMode(echoPin,INPUT);

  // led pinMode
  pinMode(redPinOut,OUTPUT);
  pinMode(greenPinOut,OUTPUT);
  pinMode(bluePinOut,OUTPUT);

  // button pinMode
  pinMode(redPinOut,OUTPUT);
  pinMode(greenPinOut,OUTPUT);
  pinMode(bluePinOut,OUTPUT);

  // Interrupt 
  attachInterrupt(IRRcvInterruptPin,IRRcvButtonOn,RISING);

  // Button Status
  buttonStatus=BUTTON_NOCHANGE;
  
  int address=0;
  int r;int i=0;
  do {
    r=Config.readinitFromEEPROM(&address,Sig[i],&SigLen[i]);      
    i++;
  }while(r>0);

  if ( r == 0 )
  {
    statusOfSystem = READY;
    SigCnt = i;
  }

  dectectStartTime = millis();
  redOldTime       = millis();
  greenOldTime     = millis();
  blueOldTime      = millis();

  if ( !Config.readVoidAt(ADDR_DISTANCE,(void*)&DISTANCE,sizeof(DISTANCE)) )
     DISTANCE = 150;

  if ( !Config.readVoidAt(ADDR_DETECT_TIME,(void*)&NOT_DETECT_TIME,sizeof(NOT_DETECT_TIME)) )
     NOT_DETECT_TIME = 5000;  // 5초
     
 // Led.DisplayOnLed(0,3);
}


/*
 *  상태 출력
*/
void BlinkLed()
{
 
   unsigned long currentTime=millis();
   if ( redDelayTime > 1 &&redDelayTime != 9999  && redDelayTime < (currentTime - redOldTime) )
   { 
      redLedStatus = (redLedStatus == HIGH ? LOW : HIGH);
      redOldTime=currentTime;
   }
   if ( greenDelayTime > 1 && greenDelayTime != 9999  && greenDelayTime < (currentTime - greenOldTime) )
   {
      greenLedStatus = (greenLedStatus == HIGH ? LOW : HIGH);
      greenOldTime=currentTime;
   }

   if ( blueDelayTime > 1 && blueDelayTime != 9999  && blueDelayTime < (currentTime - blueOldTime) )
   {
      blueLedStatus = (blueLedStatus == HIGH ? LOW : HIGH);
      blueOldTime=currentTime;
   }
           
   digitalWrite(redPinOut  ,redLedStatus);
   digitalWrite(greenPinOut,greenLedStatus); 
   
  // Serial.println("BlinkLed blueLedStatus : " +String(blueLedStatus) );   
 
   digitalWrite(bluePinOut ,blueLedStatus); 
}

void SetLedTimer(char RGB,unsigned long DelayTime)  // 0 이면 off, 1 = on, 1 보다 크면 딜레이 타임. 9999 변화 없음. 
{
  switch(RGB)
  {
    case 'R':
      redDelayTime   = DelayTime;
      if ( redDelayTime == 9999 )
         ;
      else if ( redDelayTime == 0 ) 
         redLedStatus = LOW;
      else if (redDelayTime == 1 ) 
         redLedStatus = HIGH;
      break;
    case 'G':
      greenDelayTime = DelayTime;
         
      if ( greenDelayTime == 9999 )
         ;
      else if ( greenDelayTime == 0 ) 
         greenLedStatus = LOW;
      else if (greenDelayTime == 1 ) 
         greenLedStatus = HIGH;
    
      break;
      
    case 'B':

      blueDelayTime = DelayTime;

      if ( blueDelayTime == 9999 )
         ;
      else if ( blueDelayTime == 0 ) 
         blueLedStatus = LOW;
      else if (blueDelayTime == 1 ) 
         blueLedStatus = HIGH;

      
    //  Serial.println("SetLedTimer blueLedStatus : " +String(blueLedStatus) );   
  }

  
}

void DisplayLed()
{
  if ( statusOfSystem == NOT_READY )
     SetLedTimer('R',BLINK_DELAY);
  else if ( statusOfSystem == READY )
     SetLedTimer('R',1);

  if ( statusOfDetect == DETECT )
     SetLedTimer('G',1);
  else if ( statusOfDetect == NOT_DETECT )
     SetLedTimer('G',BLINK_DELAY);

  BlinkLed();  
}

void sendIR(bool onOff)
{
  if ( onOff == OnOff )  // 현상태와 같다면 Skip
     return;

  OnOff = onOff;

           Serial.println("sendIR : " +String(onOff) );
   
  int khz = 38; // 38kHz carrier frequency for the NEC protocol
  if ( onOff )
      irsend.sendRaw(Sig[0], SigLen[0] , khz); //Note the approach used to automatically calculate the size of the array.
   else {
      for (int i=1;i<SigCnt;i++){
        irsend.sendRaw(Sig[i], SigLen[i] , khz); //Note the approach used to automatically calculate the size of the array.   
        delay(1000);
      }
   }
   SetLedTimer('B',(onOff)?1:0);
}

void checkDistance()
{

  unsigned long   currentTime =  millis();

  if ( ( currentTime - urtraChkOldTime ) < URTRA_CHK_TIME )  
     return;

  urtraChkOldTime = currentTime;    

  // put your main code here, to run repeatedly:
  digitalWrite(triggerPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin,LOW);
  distance=pulseIn(echoPin,HIGH)/58;

  float ratio = 0;
  if ( distance <= DISTANCE )
      ratio=(1-1.0*distance/DISTANCE)*8;
  Led.DispProgressBar((int)ratio);

  if ( statusOfDetect == DETECT )
  {
       if ( distance > DISTANCE )  
          notDectectCnt ++; 
       else
          DectectCnt ++; 

       if ( (currentTime- dectectStartTime ) > NOT_DETECT_TIME )
       {   
        #ifdef _DEBUG_
           Serial.println("DETECT : " +String(DectectCnt) +" " +  String(notDectectCnt) +" "+ String(1.0*notDectectCnt / (notDectectCnt+DectectCnt) ));
        #endif   
           if ( 1.0*notDectectCnt / (notDectectCnt+DectectCnt)  > DETECT_RAT )
           {
             statusOfDetect = NOT_DETECT;
             sendIR(OFF);
           }
        #ifdef _DEBUG_
           Serial.println("Distance is(cm) " + String(distance)+" "+min((int)((1-1.0*distance/DISTANCE)*8),0));
           Serial.println("DETECT(DISTANCE) (" +String(DISTANCE)+")"+String(DectectCnt) +" " +  String(notDectectCnt) +" "+ String(1.0*notDectectCnt / (notDectectCnt+DectectCnt) ));
        #endif
           dectectStartTime = currentTime;
           notDectectCnt=0;
           DectectCnt   =0;
       }

    }
    else
    {
       if ( distance <= DISTANCE )      
          DectectCnt ++; 
       else
          notDectectCnt ++; 
       if ( (currentTime- dectectStartTime ) > NOT_DETECT_TIME )
       {   
        #ifdef _DEBUG_
           Serial.println("NOT_DETECT(DISTANCE) (" +String(DISTANCE)+")"+String(DectectCnt) +" " +  String(notDectectCnt) +" "+ String(1.0*DectectCnt / (notDectectCnt+DectectCnt) ));
        #endif   

           if ( 1.0*DectectCnt / (notDectectCnt+DectectCnt)  > DETECT_RAT )
           {
             statusOfDetect = DETECT;
             sendIR(ON);
           }
           #ifdef _DEBUG_
           Serial.println("Distance is(cm) " + String(distance));
           Serial.println("NOT_DETECT(DISTANCE) (" +String(DISTANCE)+")"+String(DectectCnt) +" " +  String(notDectectCnt) +" "+ String(1.0*DectectCnt / (notDectectCnt+DectectCnt) ));
           #endif
           dectectStartTime = currentTime;
           notDectectCnt=0;
           DectectCnt   =0;
       }
  
    }
    delay(10);
}

void IRRcvButtonOn()
{

    #ifdef _DEBUG_
       Serial.println("IRRcvButtonOn 호출됨 ");
    #endif   
  
   if (millis() - IRRcvTime < 2000)
   {
      Serial.println("IRRcvButtonOn 연속 호출 < 2초 "); 
      return; 
   }
   IRRcvTime   = millis();   

  #ifdef _DEBUG_
  
     Serial.println(" IRRcvButtonOn statusOfSystem: "+String(statusOfSystem)+","+String(SigCnt));
  #endif       

   if (  statusOfSystem == IR_RCVING && SigCnt > 1 )
   {
      
      statusOfSystem = READY;
      buttonStatus   = BUTTON_NOCHANGE;
      #ifdef _DEBUG_
         Serial.println(" 리모컨 입력 완료됨(입력건수) : "+String(SigCnt));
         Serial.println(" 리모컨 입력 BUTTON_IR_RCV : "+String(buttonStatus));
         Serial.println(" IRRcvButtonOn statusOfSystem: "+String(statusOfSystem)+","+String(SigCnt));
      #endif       

      return; 
   }

   #ifdef _DEBUG_
         Serial.println(" 리모컨 입력 BUTTON_IR_RCV : "+String(buttonStatus));
   #endif       

   buttonStatus= BUTTON_IR_RCV; 
   irrecv.enableIRIn();  // Start the receiver 

   SigCnt         = 0;      // 시그날 갯수
   AddrEEPROM     = 0;      // EEOROM 주소    
}

bool readIRSig()
{  

  if ( statusOfSystem == READY )
     return false;
     
  decode_results  results;        // Somewhere to store the results
  if (irrecv.decode(&results)) {    // Grab an IR code
    for (int i = 1;  i < results.rawlen;  i++) 
       Sig[SigCnt][i-1] = results.rawbuf[i] * USECPERTICK;      
    SigLen[SigCnt] = results.rawlen-1;  
    irrecv.resume();
    SigCnt++;                    // 시그널 갯수 증가
    return true;            // 데이타를 수신중.       
  }
  return false;            
}
void loop() {
  
  // 초기화가 안되었으면 초기화 코드를 호출한다.
  while ( statusOfSystem == NOT_READY || statusOfSystem == IR_RCVING)
  {
    bool r=readIRSig();
    if ( r ){
       statusOfSystem = IR_RCVING;
       #ifdef _DEBUG_
          Serial.println("After readIRSig "+String(AddrEEPROM)+","+String(SigCnt));
       #endif        
    }

    if ( r )
    {
       #ifdef _DEBUG_
          Serial.println("IR 신호 수신및 기록(AddrEEPROM,SigCnt)"+String(AddrEEPROM)+","+String(SigCnt));
       #endif        
       Config.writeInitIntoEEPROM(&AddrEEPROM,Sig[SigCnt-1],SigLen[SigCnt-1]);
    //     delay(1000);
       irrecv.resume();
    }
  #ifdef _DEBUG_  
  //   Serial.println("loop statusOfSystem: "+String(statusOfSystem));
  //   delay(1000);
  #endif     
    DisplayLed();
    Led.DisplayOnLed(0,SigCnt);  
  }

  // IR Command 수신

  unsigned int delayTm;
  char r=irComm.getIR(irrecv,&delayTm);
  switch(r)
  {
    case 'R' : //rew
       DISTANCE -= DIST_UNIT;  // 20cm
       DISTANCE = max(DISTANCE,2);
       Config.writeVoidAt(ADDR_DISTANCE,(void*)&DISTANCE,sizeof(DISTANCE));
   //    irrecv.enableIRIn();  // Start the receiver 
       break;
    case 'F' : //FF
       DISTANCE += DIST_UNIT;  // 20cm
       DISTANCE = min(DISTANCE,400);
       Config.writeVoidAt(ADDR_DISTANCE,(void*)&DISTANCE,sizeof(DISTANCE));
 //      irrecv.enableIRIn();  // Start the receiver 
       break;
    case '+' : //delay time
       NOT_DETECT_TIME += NOT_DETECT_TIME_UNIT;  
       NOT_DETECT_TIME = min(NOT_DETECT_TIME,NOT_DETECT_TIME_MAX);
       Config.writeVoidAt(ADDR_DETECT_TIME,(void*)&NOT_DETECT_TIME,sizeof(NOT_DETECT_TIME));
   //    irrecv.enableIRIn();  // Start the receiver 
       break; 
    case '-' : //delay time
       NOT_DETECT_TIME -= NOT_DETECT_TIME_UNIT;  
       NOT_DETECT_TIME = max(NOT_DETECT_TIME,NOT_DETECT_TIME_MIN);
       Config.writeVoidAt(ADDR_DETECT_TIME,(void*)&NOT_DETECT_TIME,sizeof(NOT_DETECT_TIME));
     //  irrecv.enableIRIn();  // Start the receiver 
       break;   
    case 'M' : //delay time
       LedMode ++;
       LedMode=LedMode%2;
    //   irrecv.enableIRIn();  // Start the receiver 
       break;      
    case 'W' : //power
       OnOff = !OnOff; 
       statusOfDetect = (OnOff)?DETECT:NOT_DETECT;
        Serial.println(" loop power: "+String(statusOfDetect)+","+String(OnOff));   
       SetLedTimer('B',(OnOff)?1:0);
   //    irrecv.enableIRIn();  // Start the receiver 
       break;           
  }
  if ( (millis() - IRoldTm ) > 20000 ) {
     irrecv.enableIRIn();  // Start the receiver 
     IRoldTm = millis();     
  }
 // DISTANCE =  map(analogRead(distAnalogPin),0,1023,2,400);
  switch( LedMode )
  {
/*    case LED_DIST_ON:
        Led.DisplayOnLed(DISTANCE/100,(DISTANCE-DISTANCE/100*100)/10);
        break;*/
    case LED_DIST_LMT:
        Led.DisplayOnLed(DISTANCE/100,(DISTANCE-DISTANCE/100*100)/10);       
        break;
    case LED_DIST_TM:
        Led.DisplayOnLed(NOT_DETECT_TIME/60000,(NOT_DETECT_TIME-NOT_DETECT_TIME/60000*60000)/6000.0);  
      break;
  }


  // 버튼입력 대기
  if ( buttonStatus == BUTTON_IR_RCV )
  {
      #ifdef _DEBUG_
         Serial.println(" loop buttonStatus : "+String(buttonStatus));
         Serial.println(" loop statusOfSystem: "+String(statusOfSystem)+","+String(SigCnt));
      #endif       

     statusOfSystem = NOT_READY;
     buttonStatus   = BUTTON_NOCHANGE;
  }

  if ( statusOfSystem == READY )
  {
    
   // NOT_DETECT_TIME =  map(analogRead(delayAnalogPin),0,1023,DETECT_TIME_MIN,DETECT_TIME_MAX)*1000;
    checkDistance();
  }
  
  // NOT DECTECT 상태에서  거리가 탐지거리 이하이면 DECTECT 상태로 변경.
  DisplayLed();
   
}  

