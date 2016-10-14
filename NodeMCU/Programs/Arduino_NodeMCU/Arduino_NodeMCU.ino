void setup() {
  // put your setup code here, to run once:
  pinMode(13,OUTPUT);
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(13,HIGH);
  Serial.flush();
  String temp;
  //Tempreature
  temp = analogRead(A0);
  temp += ":";
  //Light
  temp += analogRead(A1);
  //Moisture
  temp += ":";
  temp += analogRead(A2);
  Serial.println(temp);
  delay(2000);
  digitalWrite(13,LOW);
  
}
