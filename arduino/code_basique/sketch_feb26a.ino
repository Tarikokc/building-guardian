#include <DHT.h>

#define DHTPIN D4
#define DHTTYPE DHT11 
#define PIR_PIN D2
DHT dht(DHTPIN, DHTTYPE);

void setup() {
   Serial.begin(9600);
   dht.begin();
   Serial.begin(9600);
}

void loop() {
   float h = dht.readHumidity();
   float t = dht.readTemperature();

   Serial.print("Humidity: ");
   Serial.println(h);
   Serial.print("Temperature: ");
   Serial.println(t);
   delay(1000);

   int etat = digitalRead(PIR_PIN);
   
   Serial.print("Mouvement: ");
   if (etat == HIGH) {
    Serial.println("Mouvement détecté !");
   } else {
    Serial.println("Aucun mouvement");
   }
}