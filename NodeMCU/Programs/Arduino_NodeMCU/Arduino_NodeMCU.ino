#define MAX_ADC_READING 1023.0
#define ADC_REF_VOLTAGE 5.0
#define REF_RESISTANCE 10000.0    //10k ohms 
#define LUX_CALC_SCALAR 12518931
#define LUX_CALC_EXPONENT -1.405

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

  //Light
  float ldrRawData = analogRead(A1);
  // MAX_ADC_READING is 1023 and ADC_REF_VOLTAGE is 5
  float resistorVoltage = (float)ldrRawData / MAX_ADC_READING * ADC_REF_VOLTAGE;
  //Since 5V is split between the 5 kohm resistor and the LDR, simply subtract the resistor voltage from 5V:
  float ldrVoltage = ADC_REF_VOLTAGE - resistorVoltage;
  //The resistance of the LDR must be calculated based on the voltage (simple resistance calculation for a voltage divider circuit):
  float ldrResistance = ldrVoltage/resistorVoltage * REF_RESISTANCE;
  float ldrLux = LUX_CALC_SCALAR * pow(ldrResistance, LUX_CALC_EXPONENT);  
  
  //Tempreature
  float tempreature = (ADC_REF_VOLTAGE * analogRead(A0) * 100.0) / MAX_ADC_READING;
  
  //Tempreature
  String temp = (String)tempreature;
  temp += ":";
  //Light
  temp += ldrLux;
  //Moisture
  temp += ":";
  temp += analogRead(A2);
  Serial.println(temp);
  delay(2000);
  digitalWrite(13,LOW);
  delay(5000);
}
