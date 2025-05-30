#include <Arduino.h>
#include <DHT.h>
#include "Arduino_H7_Video.h"
#include "Arduino_GigaDisplayTouch.h"
#include "lvgl.h"

#define DHTPIN 2
#define DHTTYPE DHT22
#define SOIL_PIN A0
#define LDR_PIN A2

DHT dht(DHTPIN, DHTTYPE);
Arduino_H7_Video Display(800, 480, GigaDisplayShield);
Arduino_GigaDisplayTouch TouchDetector;

lv_obj_t *tempLabel, *humLabel, *soilLabel, *lightLabel;

float temperature = 0.0, humidity = 0.0;
int soilMoisture = 0, lightLevel = 0;

void readSensors() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  soilMoisture = analogRead(SOIL_PIN);
  lightLevel = analogRead(LDR_PIN);
}

void updateDisplay() {
  String tempText = "Temp: " + String(temperature, 1) + " C";
  String humText = "Humidity: " + String(humidity, 1) + " %";

  lv_label_set_text(tempLabel, tempText.c_str());
  lv_label_set_text(humLabel, humText.c_str());
  lv_label_set_text_fmt(soilLabel, "Soil: %d", soilMoisture);
  lv_label_set_text_fmt(lightLabel, "Light: %d", lightLevel);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(SOIL_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);

  Display.begin();
  TouchDetector.begin();

  lv_obj_t *screen = lv_scr_act();
  static lv_coord_t col_dsc[] = { 400, LV_GRID_TEMPLATE_LAST };
  static lv_coord_t row_dsc[] = { 80, 80, 80, 80, LV_GRID_TEMPLATE_LAST };
  lv_obj_t *grid = lv_obj_create(screen);
  lv_obj_set_grid_dsc_array(grid, col_dsc, row_dsc);
  lv_obj_set_size(grid, Display.width(), Display.height());
  lv_obj_center(grid);

  tempLabel = lv_label_create(grid);
  lv_obj_set_grid_cell(tempLabel, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
  humLabel = lv_label_create(grid);
  lv_obj_set_grid_cell(humLabel, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
  soilLabel = lv_label_create(grid);
  lv_obj_set_grid_cell(soilLabel, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
  lightLabel = lv_label_create(grid);
  lv_obj_set_grid_cell(lightLabel, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
}

void loop() {
  readSensors();
  updateDisplay();
  lv_timer_handler();
  delay(2000);
}
