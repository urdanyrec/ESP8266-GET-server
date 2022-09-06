#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "DHT.h"
DHT dht(D2, DHT11);
const char* ssid = "Чипирование";
const char* password = "03082000"; 
ESP8266WebServer server(80);

void getAPI() 
{
  //float h = dht.readHumidity();     // get humidity
  //float t = dht.readTemperature();  // get temperature
  String Json = "{\"type\": \"Temperature\", \"dimension\": \"C\", \"value\": \"" + String(dht.readTemperature()) + "\"}, {\"type\": \"Humidity\", \"dimension\": \"%\", \"value\": \"" + String(dht.readHumidity()) + "\"}";
  String JsonList = "[" + Json + "]";
  server.send(200, "application/json", JsonList);
}

void getUI() 
{
  float h = dht.readHumidity();     // get humidity
  float t = dht.readTemperature();  // get temperature
  String page = "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><meta http-equiv=\"Content-Security-Policy\" content=\"upgrade-insecure-requests\"><title>ESP8266</title></head><body><h1>Сервер на ESP8266</h1><h2>Температура: ";
  page += t;
  page += " C</h2>";
  page += "<h2>Влажность: ";
  page += h;
  page += " %</h2>";
  server.send(200, "text/html", page);
}
 
// Define routing
void restServerRouting() 
{ 
    server.on(F("/api"), HTTP_GET, getAPI);
    server.on(F("/ui"), HTTP_GET, getUI);
}
 
// Manage not found URL
void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
 
void setup(void) 
{
  Serial.begin(115200);
  dht.begin();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin("esp8266")) 
  {
    Serial.println("MDNS responder started");
  }
 
  // Set server routing
  restServerRouting();
  // Set not found response
  server.onNotFound(handleNotFound);
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void) 
{
  restServerRouting();
  server.handleClient();
  delay(3000);
}
