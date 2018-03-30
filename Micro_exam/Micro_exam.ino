int distance;
int triggerPin=7;
int echoPin=6;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(triggerPin,OUTPUT);
  pinMode(echoPin,INPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(triggerPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin,LOW);

  distance=pulseIn(echoPin,HIGH)/58;

  Serial.println("Distance is(cm) " + String(distance));

  if ( accDistanceCnt >  
  
  delay(100);
}
