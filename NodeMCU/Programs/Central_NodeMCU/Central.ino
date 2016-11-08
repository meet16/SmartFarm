#include <dummy.h>

#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
SoftwareSerial toArduino(13, 15);

const char *ssid = "SmartFarm";
const char *password = "A3B4NJshb*903dfnHx";

IPAddress local_IP(192,168,0,1);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);

unsigned int localPort = 9999;      // local port to listen on

String temp;
char packetBuffer[100]; //buffer to hold incoming packet
char  ReplyBuffer[100];       // a string to send back

WiFiUDP Udp;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  toArduino.begin(9600);
  
  Serial.println();
  pinMode(LED_BUILTIN, OUTPUT);
 
  //Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  Serial.println("\nStarting UDP...");
  Udp.begin(9999);
}

void loop() {

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
    String temp = String(packetBuffer);
    digitalWrite(LED_BUILTIN, LOW);
    //String temp = Serial.readString();
    toArduino.println(temp);
    Serial.println(packetBuffer);
    
  }
  delay(2000);
  digitalWrite(LED_BUILTIN, HIGH);
}

