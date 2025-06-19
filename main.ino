#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define SOIL_PIN A0
#define LDR_PIN A2

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int soilMoisture = analogRead(SOIL_PIN);
  int lightLevel = analogRead(LDR_PIN);

  if (!isnan(temperature) && !isnan(humidity)) {
    Serial.print("DATA:");
    Serial.print(temperature, 2);
    Serial.print(",");
    Serial.print(humidity, 2);
    Serial.print(",");
    Serial.print(soilMoisture);
    Serial.print(",");
    Serial.println(lightLevel);
  } else {
    Serial.println("DATA:NaN,NaN,0,0");
  }

  delay(1000);
}
