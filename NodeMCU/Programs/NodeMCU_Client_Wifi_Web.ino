#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>1


const char *ssid = "SmartFarm";
const char *password = "A3B4NJshb*903dfnHx";
HTTPClient http;
StaticJsonBuffer<100>  jsonBuffer;
JsonObject& root = jsonBuffer.createObject();
String temp;

void receiveHttp(int httpCode){
  if(httpCode == 200) {
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.print("[HTTP] GET... failed, no connection or no HTTP server\n");
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("proyecto inv client-server");
  Serial.print("Connecting to ");
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
}

void loop(){
  /*if(!WiFi.isConnected()){
    Serial.printf("Reconnecting to SSID :%s\n",ssid);
    delay(500);
  }else{*/
    //HTTPClient http;
    root["tempreature"] = analogRead(A0);
    root["light"] = 799;
    root["moisture"] = 799;
    root.printTo(temp);
    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin("192.168.4.1", 80, "/led0"); //HTTP
    //receiveHttp(http.GET());
    delay(1000);
    http.begin("192.168.4.1", 80, "/led1"); //HTTP
    //receiveHttp(http.GET());
    delay(1000);
    http.begin("192.168.4.1", 80, "/data"); //HTTP
    //receiveHttp(http.POST(temp));
    delay(1000);
  //}
}
