#include <SoftwareSerial.h>
SoftwareSerial gprsSerial(7, 8);

void setup()
{
  gprsSerial.begin(19200);
  Serial.begin(19200);

  Serial.println("Config SIM900...");
  delay(2000);
  Serial.println("Done!...");
  gprsSerial.flush();
  Serial.flush();

  // attach or detach from GPRS service 
  gprsSerial.println("AT+CGATT?");
  delay(100);
  toSerial();


  // bearer settings
  gprsSerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
  delay(2000);
  toSerial();

  // bearer settings
  gprsSerial.println("AT+SAPBR=3,1,\"APN\",\"T24.Internet\"");
  delay(2000);
  toSerial();

  // bearer settings
  gprsSerial.println("AT+SAPBR=1,1");
  delay(2000);
  toSerial();
}


void loop()
{
  int allDone = 0;
  while(true){
    int count = 0;
    while(Serial.available() == 0){
      count++;
      //Some value will find it after experiment
      if(count == 1000){
        allDone = 1;
      }
    }
    if(allDone!=1){
      String readings = Serial.readString();
      Serial.println(readings);

      int firstDelim = readings.indexOf(';');
      int tempValue = readings.substring(0,firstDelim).toInt();
      int secondDelim = readings.indexOf(';',firstDelim+1);
      int lightValue = readings.substring(firstDelim+1,secondDelim).toInt();
      int moistureValue =  readings.substring(secondDelim+1).toInt();

      Serial.println(tempValue);
      Serial.println(lightValue);
      Serial.println(moistureValue);

      //Save in ArrayList kind of stuff
      
    }
    else
      break;
  }
  findAvg();
  if(takeDecision() == 1)
    GSMSend();  
}

void findAvg(){
  //Take all the arraylist entry and find avg. Store in global variables.
}

int takeDecision(){
  //based on threshold value give 1 or 0 as output
  return 1;
}
void GSMSend(){
  // initialize http service
   gprsSerial.println("AT+HTTPINIT");
   delay(2000); 
   toSerial();

   // set http param value
   //use global variable here to send data
   //Will need some token for unique identity of farm....so here think abt sqlite
   gprsSerial.println("AT+HTTPPARA=\"URL\",\"http://helpinghandgj100596.comule.com/test.php?temp=1&light=2&moisture=2\"");
   delay(2000);
   toSerial();

   // set http action type 0 = GET, 1 = POST, 2 = HEAD
   gprsSerial.println("AT+HTTPACTION=0");
   delay(6000);
   toSerial();

   // read server response
   gprsSerial.println("AT+HTTPREAD"); 
   delay(1000);
   toSerial();

   gprsSerial.println("");
   gprsSerial.println("AT+HTTPTERM");
   toSerial();
   delay(300);

   gprsSerial.println("");
   delay(10000);
}
void toSerial()
{
  while(gprsSerial.available()!=0)
  {
    Serial.write(gprsSerial.read());
  }
}
