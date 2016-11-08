#include <dummy.h>

#include <SoftwareSerial.h>
SoftwareSerial fromArduino(12, 13); // RX, TX

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

WiFiClient client;
WiFiUDP port;
WiFiUDP port2;

const char *ssid = "SmartFarm";
const char *password = "A3B4NJshb*903dfnHx";

unsigned int localPort = 9999;      // local port to listen on
IPAddress serverIP(192, 168, 0, 1); 
IPAddress localIPAddress;

IPAddress staticIP(192,168,0,100);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);


char packetBuffer[100]; //buffer to hold incoming packet
char  ReplyBuffer[100];       // a string to send back

int counter = 0;
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  pinMode(13,OUTPUT);
  delay(1000);
  Serial.begin(9600);
  delay(10);
  Serial.print("\nConnecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);  
  //WiFi.begin(ssid, password);
  WiFi.config(staticIP, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("\nStarting UDP...");
  // if you get a connection, report back via serial:
  localIPAddress = WiFi.localIP();
  port.begin(9999);
  fromArduino.begin(9600);

}

void loop() {
  if(!WiFi.isConnected()){
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.printf("Reconnecting to SSID :%s\n",ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected");
    delay(500);
  }
  else {
    int packetSize = port.parsePacket();
    //Serial.println(".");
    if (fromArduino.available()){
      digitalWrite(LED_BUILTIN, LOW);
      fromArduino.readString().toCharArray(packetBuffer,100);
      sendMessage(packetBuffer);
      for (int i = 0; i<100; i++)
        packetBuffer[i] = '\0';   
      fromArduino.flush();
    }
    else if (packetSize) {
      Serial.println(packetSize);
      int len = port.read(packetBuffer, 100);
      if (len > 0) 
        packetBuffer[len] = 0;
      Serial.println(packetBuffer);
      if (localIPAddress != port.remoteIP())
        sendMessage(packetBuffer);
    }
    else if (Serial.available()){
      digitalWrite(LED_BUILTIN, LOW);
      Serial.readString().toCharArray(packetBuffer,100);
      sendMessage(packetBuffer);
      for (int i = 0; i<100; i++)
        packetBuffer[i] = '\0';   
      Serial.flush();
    }
  }
  delay(2000);
}

void sendMessage(char data[]) {
  port2.beginPacket(serverIP,localPort);
  port2.write(data);
  Serial.println(data);
  port2.endPacket();
}



