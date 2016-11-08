#define MAX_ADC_READING 1023.0
#define ADC_REF_VOLTAGE 5.0
#define REF_RESISTANCE 10000.0    //10k ohms 
#define LUX_CALC_SCALAR 12518931
#define LUX_CALC_EXPONENT -1.405
#include <SoftwareSerial.h>

SoftwareSerial toNodeMCU(8, 9); // RX, TX

byte ldrOut = 10;
byte lm34Out = 11;
byte moistureOut = 12;
byte led = 13;

void setup() {
  // put your setup code here, to run once:
  pinMode(led,OUTPUT);
  pinMode(ldrOut,OUTPUT);
  pinMode(lm34Out,OUTPUT);
  pinMode(moistureOut,OUTPUT);
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  
  digitalWrite(ldrOut,LOW);
  digitalWrite(lm34Out,LOW);
  digitalWrite(moistureOut,LOW);
  
  Serial.begin(9600);
  toNodeMCU.begin(9600);
}

void loop() {
  digitalWrite(led,HIGH);
  Serial.flush();
  
  //Serial.println("Reading");
  //Light
  digitalWrite(ldrOut,HIGH);
  delay(2000);
  float ldrRawData = analogRead(A0);
  digitalWrite(ldrOut,LOW);
  delay(2000);
  // MAX_ADC_READING is 1023 and ADC_REF_VOLTAGE is 5
  float resistorVoltage = (float)ldrRawData / MAX_ADC_READING * ADC_REF_VOLTAGE;
  //Since 5V is split between the 5 kohm resistor and the LDR, simply subtract the resistor voltage from 5V:
  float ldrVoltage = ADC_REF_VOLTAGE - resistorVoltage;
  //The resistance of the LDR must be calculated based on the voltage (simple resistance calculation for a voltage divider circuit):
  float ldrResistance = ldrVoltage/resistorVoltage * REF_RESISTANCE;
  float ldrLux = LUX_CALC_SCALAR * pow(ldrResistance, LUX_CALC_EXPONENT);  
  
  //Tempreature
  digitalWrite(lm34Out,HIGH);
  delay(2000);
  float tempreature = (ADC_REF_VOLTAGE * analogRead(A1) * 100.0) / MAX_ADC_READING;
  digitalWrite(lm34Out,LOW);
  delay(2000);
  //Tempreature
  String temp = (String)tempreature;
  temp += ":";
  //Light
  temp += ldrLux;
  //Moisture
  temp += ":";
  digitalWrite(moistureOut,HIGH);
  delay(2000);
  temp += analogRead(A2);
  digitalWrite(moistureOut,LOW);
  Serial.println(temp);
  toNodeMCU.println(temp);
  delay(2000);
  digitalWrite(led,LOW);
}
