#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

int status = WL_IDLE_STATUS;
const char *ssid = "SmartFarm";
const char *password = "A3B4NJshb*903dfnHx";

IPAddress local_IP(192,168,0,1);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);

unsigned int localPort = 2390;      // local port to listen on

String temp;
char packetBuffer[100]; //buffer to hold incoming packet
char  ReplyBuffer[100];       // a string to send back
StaticJsonBuffer<100>  jsonBuffer;

WiFiUDP Udp;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  Serial.println("\nStarting UDP...");
  // if you get a connection, report back via serial:
  Udp.begin(localPort);
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
    JsonObject& root = jsonBuffer.parseObject(packetBuffer);

    if (len > 0) {
      packetBuffer[len] = 0;
    }
    if (!root.success()){
      Serial.println("parseObject() failed");
      return;
    }
    else {
      Serial.println("Contents:");
      root.prettyPrintTo(Serial);
    }
    //Serial.println("Contents:");
    //Serial.println(packetBuffer);

    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    temp = "OK";
    temp.toCharArray(ReplyBuffer,100);
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
}
