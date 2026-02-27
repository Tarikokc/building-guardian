#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>


#define DHTPIN D4
#define DHTTYPE DHT11 
#define PIR_PIN D2

const char* ssid = "iPhone de Alyssa";
const char* password = "azerty123";
const char* mqtt_server = "172.20.10.3"; 

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

String salleID;


void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(PIR_PIN, INPUT);
  dht.begin();

  Serial.println(WiFi.scanNetworks());

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connecté");
  
  salleID = "salle" + WiFi.macAddress().substring(15);

  client.setServer(mqtt_server, 1883);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connexion MQTT...");
    if (client.connect((salleID + "_client").c_str())) {
      Serial.println("MQTT connecté");
    } else {
      delay(2000);
    }
  }
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  float hum = dht.readHumidity();
  float temp = dht.readTemperature();
  int pir = digitalRead(PIR_PIN);

  String json = "{";
  json += "\"temp\":" + String(temp, 1) + ",";
  json += "\"hum\":" + String(hum, 1) + ",";
  json += "\"pir\":" + String(pir);
  json += "}";

  if (client.publish(salleID.c_str(), json.c_str())) {
    Serial.println("✓ " + salleID + ": " + json);
  } else {
    Serial.println("✗ MQTT");
  }
  delay(5000);

}