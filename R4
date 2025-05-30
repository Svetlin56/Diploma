#include <DHT.h>

#define DHTPIN 2         // DHT22 пин
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define SOIL_PIN A0      // Почвена влажност
#define LDR_PIN A2       // Осветеност

void setup() {
  Serial.begin(9600);    // Съответства на Node.js
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
    Serial.println(lightLevel); // println добавя \n
  } else {
    Serial.println("DATA:NaN,NaN,0,0"); // За да не се чупи парсера
  }

  delay(10000); // на всеки 10 секунди
}
