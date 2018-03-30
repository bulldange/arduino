void setup() {
  // put your setup code here, to run once:
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(2,(random(2)==0)?HIGH:LOW);
  digitalWrite(3,(random(2)==0)?HIGH:LOW);
  digitalWrite(4,(random(2)==0)?HIGH:LOW);
  delay(1000);
  digitalWrite(2,(random(2)==0)?HIGH:LOW);
  digitalWrite(3,(random(2)==0)?HIGH:LOW);
  digitalWrite(4,(random(2)==0)?HIGH:LOW);
  delay(1000);
  digitalWrite(2,(random(2)==0)?HIGH:LOW);
  digitalWrite(3,(random(2)==0)?HIGH:LOW);
  digitalWrite(4,(random(2)==0)?HIGH:LOW);
  delay(1000);

}
