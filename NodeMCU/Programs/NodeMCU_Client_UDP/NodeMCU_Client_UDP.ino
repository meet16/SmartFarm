#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

int status = WL_IDLE_STATUS;
const char *ssid = "SmartFarm";
const char *password = "A3B4NJshb*903dfnHx";
StaticJsonBuffer<100>  jsonBuffer;
JsonObject& root = jsonBuffer.createObject();

unsigned int localPort = 2390;      // local port to listen on
IPAddress receiver(192, 168, 0, 1); 

String temp;
char packetBuffer[100]; //buffer to hold incoming packet
char  ReplyBuffer[100];       // a string to send back

WiFiUDP Udp;

void setup() {
  //Initialize serial and wait for port to open:
  delay(1000);
  Serial.begin(115200);
  delay(10);
  Serial.print("\nConnecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);   
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("\nStarting UDP...");
  // if you get a connection, report back via serial:
  Udp.begin(localPort);
  Udp.beginPacket(receiver, localPort);
  root["tempreature"] = 799;
  root["light"] = 799;
  root["moisture"] = 799;
  root.printTo(temp);
  temp.toCharArray(ReplyBuffer,100);
  Udp.write(ReplyBuffer);
  Udp.endPacket();
}

void loop() {
  if(!WiFi.isConnected()){
    Serial.printf("Reconnecting to SSID :%s\n",ssid);
    WiFi.begin(ssid, password);
    delay(500);
  }
  else {
    // if there's data available, read a packet
    int packetSize = Udp.parsePacket();
    if (packetSize) {
      Serial.print("Received packet of size ");
      Serial.println(packetSize);
      Serial.print("From ");
      IPAddress remoteIp = Udp.remoteIP();
      Serial.print(remoteIp);
      Serial.print(", port ");
      Serial.println(Udp.remotePort());
  
      // read the packet into packetBufffer
      int len = Udp.read(packetBuffer, 100);
      if (len > 0) {
        packetBuffer[len] = 0;
      }
      Serial.println("Contents:");
      Serial.println(packetBuffer);
    }
    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(receiver, localPort);
    root["tempreature"] = 799;
    root["light"] = 799;
    root["moisture"] = 799;
    root.printTo(temp);
    temp.toCharArray(ReplyBuffer,100);
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
}
