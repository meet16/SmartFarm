#include <ESP8266WiFi.h>
#include<WiFiClient.h> 
#include<ESP8266WebServer.h>`

IPAddress local_IP(192,168,0,1);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);

const char *ssid = "SmartFarm";
const char *password = "A3B4NJshb*903dfnHx";

ESP8266WebServer server(80);

void handle_led1(){
  digitalWrite(LED_BUILTIN, 1);
  server.send(200, "text/plain", String("LED ON ")); 
}

void handle_led0(){
  digitalWrite(LED_BUILTIN, 0);
  server.send(200, "text/plain", String("LED OFF ")); 
}

void handle_msg(){
  server.send(200, "text/plain", String("TRUE")); 
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  Serial.println("HTTP server started");
  server.on("/led1",handle_led1);
  server.on("/led0",handle_led0);
  server.on("/data", [](){
    Serial.println(server.uri());
    server.send(200, "text/plain", "this works as well");
  });
  server.on("/msg",handle_msg);
}

void loop() {
  server.handleClient();
}
