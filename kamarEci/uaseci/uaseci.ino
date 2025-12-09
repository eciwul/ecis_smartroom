#include <WiFi.h>
#include "DHT.h"

#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int relayPin = 4;
const int ledUtama = 12;
const int ledBelajar = 14;
const int ledTidur = 27;

bool fanState = false;
bool utamaState = false;
bool belajarState = false;
bool tidurState = false;

const char* ssid = "Apoehh";
const char* password = "duasatudua";
WiFiServer server(80);

void sendCORS(WiFiClient &client) {
  client.println("Access-Control-Allow-Origin: *");
  client.println("Access-Control-Allow-Methods: GET, POST, OPTIONS");
  client.println("Access-Control-Allow-Headers: *");
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(relayPin, OUTPUT);
  pinMode(ledUtama, OUTPUT);
  pinMode(ledBelajar, OUTPUT);
  pinMode(ledTidur, OUTPUT);

  digitalWrite(relayPin, HIGH); 
  digitalWrite(ledUtama, LOW);
  digitalWrite(ledBelajar, LOW);
  digitalWrite(ledTidur, LOW);

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
  server.begin();
}

void handleRequest(String req) {
  if(req.indexOf("/fan/on")>=0){digitalWrite(relayPin, LOW); fanState=true;}
  if(req.indexOf("/fan/off")>=0){digitalWrite(relayPin, HIGH); fanState=false;}

  if(req.indexOf("/utama/on")>=0){digitalWrite(ledUtama, HIGH); utamaState=true;}
  if(req.indexOf("/utama/off")>=0){digitalWrite(ledUtama, LOW); utamaState=false;}

  if(req.indexOf("/belajar/on")>=0){digitalWrite(ledBelajar, HIGH); belajarState=true;}
  if(req.indexOf("/belajar/off")>=0){digitalWrite(ledBelajar, LOW); belajarState=false;}

  if(req.indexOf("/tidur/on")>=0){digitalWrite(ledTidur, HIGH); tidurState=true;}
  if(req.indexOf("/tidur/off")>=0){digitalWrite(ledTidur, LOW); tidurState=false;}
}

void sendStatus(WiFiClient &client){
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  client.println("HTTP/1.1 200 OK");
  sendCORS(client);
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println();

  client.print("{");
  client.print("\"temperature\":"); client.print(temp); client.print(",");
  client.print("\"humidity\":"); client.print(hum); client.print(",");
  client.print("\"fan\":"); client.print(fanState ? "true":"false"); client.print(",");
  client.print("\"utama\":"); client.print(utamaState ? "true":"false"); client.print(",");
  client.print("\"belajar\":"); client.print(belajarState ? "true":"false"); client.print(",");
  client.print("\"tidur\":"); client.print(tidurState ? "true":"false");
  client.println("}");
}

void sendOptions(WiFiClient &client) {
  client.println("HTTP/1.1 204 No Content");
  sendCORS(client);
  client.println("Connection: close");
  client.println();
}

void loop() {
  WiFiClient client = server.available();
  if(client){
    String req = client.readStringUntil('\r');
    client.read(); 

    if(req.indexOf("OPTIONS") >= 0){
      sendOptions(client);
      client.stop();
      return;
    }

    handleRequest(req);

    if(req.indexOf("/status") >= 0) {
      sendStatus(client);
    } else {
      client.println("HTTP/1.1 200 OK");
      sendCORS(client);
      client.println("Content-Type: text/html");
      client.println("Connection: close");
      client.println();
      client.println("ESP32 Online");
    }

    delay(1);
    client.stop();
  }
}
