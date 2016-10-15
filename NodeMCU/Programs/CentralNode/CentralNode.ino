#include <SoftwareSerial.h>
#define TOTALNODE 10

SoftwareSerial gprsSerial(7, 8);

float finalTemp, finalLight,finalMoisture;

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
  int savedTemp[TOTALNODE],savedLight[TOTALNODE],savedMoisture[TOTALNODE];
  senseSensors(savedTemp,savedLight,savedMoisture);
  findAvg(savedTemp,savedLight,savedMoisture);
  if(takeDecision() == 1){
    //Starttimer
    while(takeDecision() == 1){
      senseSensors(savedTemp,savedLight,savedMoisture);
      findAvg(savedTemp,savedLight,savedMoisture);
    }
    //Stop Timer
  }
    GSMSend();//Send Time as parameter also.  
}

void senseSensors(int savedTemp[],int savedLight[],int savedMoisture[]){

   // Other way... When u know no of nodes
  // To have this, while setting up the central node, u need to define the variable.
  int i=0;
  
  for(i=0;i<TOTALNODE;i++){
    
      while(Serial.available() == 0);
      String readings = Serial.readString();
      Serial.println(readings);

      int firstDelim = readings.indexOf(':');
      int tempValue = readings.substring(0,firstDelim).toInt();
      int secondDelim = readings.indexOf(':',firstDelim+1);
      int lightValue = readings.substring(firstDelim+1,secondDelim).toInt();
      int moistureValue =  readings.substring(secondDelim+1).toInt();

      Serial.println(tempValue);
      Serial.println(lightValue);
      Serial.println(moistureValue);
      
      savedTemp[i] = tempValue;
      savedLight[i] = lightValue;
      savedMoisture[i] = moistureValue;

      Serial.flush();
  }
  
  /*
  // One way t do this
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

      int firstDelim = readings.indexOf(':');
      int tempValue = readings.substring(0,firstDelim).toInt();
      int secondDelim = readings.indexOf(':',firstDelim+1);
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
  */
}

void findAvg(int savedTemp[],int savedLight[],int savedMoisture[]){
  int i=0;
  int temp,light,moisture;
  temp=light=moisture=0;
  for(i=0;i<TOTALNODE;i++){
    temp+=savedTemp[i];
    light+=savedLight[i];
    moisture+=savedMoisture[i];
  }

  finalTemp = temp/float(TOTALNODE);
  finalLight = light/float(TOTALNODE);
  finalMoisture = moisture/float(TOTALNODE);
}

int takeDecision(){
  /*
   * based on threshold value give 1 or 0 as output
  //Some thing has to be found which can relate temperature and light with moisture
  
  if(finalMoisture < THRESHOLD_MOISTURE)
    //turn motor on;

  */
  
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
