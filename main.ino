#include <Arduino.h>
#include <DHT.h>
#include "arducam_dvp.h"
#include "Arduino_H7_Video.h"
#include "dsi.h"
#include "SDRAM.h"
#include <PubSubClient.h>

#define ARDUCAM_CAMERA_HM0360

#ifdef ARDUCAM_CAMERA_HM01B0
#include "Himax_HM01B0/himax.h"
HM01B0 himax;
Camera cam(himax);

#define IMAGE_MODE CAMERA_GRAYSCALE
#elif defined(ARDUCAM_CAMERA_HM0360)
#include "Himax_HM0360/hm0360.h"
HM0360 himax;
Camera cam(himax);

#define IMAGE_MODE CAMERA_GRAYSCALE
#elif defined(ARDUCAM_CAMERA_OV767X)
#include "OV7670/ov767x.h"
OV7675 ov767x;
Camera cam(ov767x);

#define IMAGE_MODE CAMERA_RGB565
#elif defined(ARDUCAM_CAMERA_GC2145)
#include "GC2145/gc2145.h"
GC2145 galaxyCore;
Camera cam(galaxyCore);

#define IMAGE_MODE CAMERA_RGB565
#endif

FrameBuffer fb;
FrameBuffer outfb;
Arduino_H7_Video Display(800, 480, GigaDisplayShield);

#define DHTPIN 2
#define DHTTYPE DHT22
#define SOIL_PIN A0
#define SOLAR_PIN A1
#define LDR_PIN A2
#define REFERENCE_VOLTAGE 3.3
#define VOLTAGE_DIVIDER_RATIO 2.0

DHT dht(DHTPIN, DHTTYPE);
float temperature = 0.0, humidity = 0.0, solarVoltage = 0.0;
int soilMoisture = 0, lightLevel = 0;

void blinkLED(uint32_t count = 0xFFFFFFFF)
{
  pinMode(LED_BUILTIN, OUTPUT);
  while (count--) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
  }
}

uint32_t palette[256];


void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(SOIL_PIN, INPUT);
  pinMode(SOLAR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  delay(3000);

  if (!cam.begin(CAMERA_R320x240, IMAGE_MODE, 30)) {
    blinkLED();
  }

  for (int i = 0; i < 256; i++) {
    palette[i] = 0xFF000000 | (i << 16) | (i << 8) | i;
  }

  Display.begin();

  if (IMAGE_MODE == CAMERA_GRAYSCALE) {
    dsi_configueCLUT((uint32_t*)palette);
  }
  outfb.setBuffer((uint8_t*)SDRAM.malloc(1024 * 1024));
  dsi_lcdClear(0);
  dsi_drawCurrentFrameBuffer();
  dsi_lcdClear(0);
  dsi_drawCurrentFrameBuffer();
}

#define HTONS(x)    (((x >> 8) & 0x00FF) | ((x << 8) & 0xFF00))


void readSensors() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  soilMoisture = analogRead(SOIL_PIN);
  int raw = analogRead(SOLAR_PIN);
  solarVoltage = (raw / 1023.0) * REFERENCE_VOLTAGE * VOLTAGE_DIVIDER_RATIO;
  lightLevel = analogRead(LDR_PIN);
}

void logSensorData() {
  Serial.print("DATA:");
  Serial.print(temperature); Serial.print(",");
  Serial.print(humidity); Serial.print(",");
  Serial.print(soilMoisture); Serial.print(",");
  Serial.print(lightLevel); Serial.print(",");
  Serial.println(solarVoltage, 2);
}


void loop() {
  readSensors();
  logSensorData();

  if (cam.grabFrame(fb, 3000) == 0) {

    for (int i = 0; i < 320; i++) {
      for (int j = 0; j < 240; j++) {
        if (IMAGE_MODE == CAMERA_GRAYSCALE) {
          ((uint8_t*)outfb.getBuffer())[j * 2 + (i * 2) * 480] = ((uint8_t*)fb.getBuffer())[i + j * 320];
          ((uint8_t*)outfb.getBuffer())[j * 2 + (i * 2) * 480 + 1] = ((uint8_t*)fb.getBuffer())[i + j * 320];
          ((uint8_t*)outfb.getBuffer())[j * 2 + (i * 2 + 1) * 480] = ((uint8_t*)fb.getBuffer())[i + j * 320];
          ((uint8_t*)outfb.getBuffer())[j * 2 + (i * 2 + 1) * 480 + 1] = ((uint8_t*)fb.getBuffer())[i + j * 320];
        } else {
          ((uint16_t*)outfb.getBuffer())[j * 2 + (i * 2) * 480] = HTONS(((uint16_t*)fb.getBuffer())[i + j * 320]);
          ((uint16_t*)outfb.getBuffer())[j * 2 + (i * 2) * 480 + 1] = HTONS(((uint16_t*)fb.getBuffer())[i + j * 320]);
          ((uint16_t*)outfb.getBuffer())[j * 2 + (i * 2 + 1) * 480] = HTONS(((uint16_t*)fb.getBuffer())[i + j * 320]);
          ((uint16_t*)outfb.getBuffer())[j * 2 + (i * 2 + 1) * 480 + 1] = HTONS(((uint16_t*)fb.getBuffer())[i + j * 320]);
        }
      }
    }
    dsi_lcdDrawImage((void*)outfb.getBuffer(), (void*)dsi_getCurrentFrameBuffer(), 480, 640, IMAGE_MODE == CAMERA_GRAYSCALE ? DMA2D_INPUT_L8 : DMA2D_INPUT_RGB565);
    dsi_drawCurrentFrameBuffer();
  } else {
    blinkLED(20);
  }

  delay(10000);
}
