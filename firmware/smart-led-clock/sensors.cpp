/**
 * Smart LED Clock - Sensors Module Implementation
 * 
 * Author: F. Baillon
 * Version: Phase 5
 * Date: January 2025
 * License: GPL v3.0
 */

#include "sensors.h"
#include "leds.h"
#include <Arduino.h>

// ==========================================
// GLOBAL SENSOR OBJECTS
// ==========================================
DHT dhtIndoor(PIN_DHT_INDOOR, DHT_TYPE);
DHT dhtOutdoor(PIN_DHT_OUTDOOR, DHT_TYPE);

// ==========================================
// FUNCTION IMPLEMENTATIONS
// ==========================================

void initSensors() {
  // Initialize DHT22 sensors
  dhtIndoor.begin();
  dhtOutdoor.begin();
  Serial.println("DHT22 sensors initialized");
  
  // Initialize air quality sensor
  pinMode(PIN_AIR_QUALITY_SENSOR, INPUT);
  Serial.println("MQ135 air quality sensor initialized");
}

void updateSensorData() {
  // Read indoor sensor
  float tempIn = dhtIndoor.readTemperature();
  float humIn = dhtIndoor.readHumidity();
  
  if (!isnan(tempIn) && !isnan(humIn)) {
    indoorData.temperature = tempIn;
    indoorData.humidity = humIn;
    indoorData.feelsLike = dhtIndoor.computeHeatIndex(tempIn, humIn, false);
    indoorData.dewPoint = calculateDewPoint(tempIn, humIn);
    indoorData.humidex = calculateHumidex(tempIn, humIn);
    indoorData.valid = true;
    indoorData.lastUpdate = millis();
  } else {
    indoorData.valid = false;
    Serial.println("ERROR: Indoor sensor read failed");
  }

  // Read outdoor sensor
  float tempOut = dhtOutdoor.readTemperature();
  float humOut = dhtOutdoor.readHumidity();
  
  if (!isnan(tempOut) && !isnan(humOut)) {
    outdoorData.temperature = tempOut;
    outdoorData.humidity = humOut;
    outdoorData.feelsLike = dhtOutdoor.computeHeatIndex(tempOut, humOut, false);
    outdoorData.dewPoint = calculateDewPoint(tempOut, humOut);
    outdoorData.humidex = calculateHumidex(tempOut, humOut);
    outdoorData.valid = true;
    outdoorData.lastUpdate = millis();
  } else {
    outdoorData.valid = false;
    Serial.println("ERROR: Outdoor sensor read failed");
  }
}

float calculateDewPoint(float temp, float humidity) {
  float alpha = log(humidity / 100.0) + (17.27 * temp) / (237.3 + temp);
  return (237.3 * alpha) / (17.27 - alpha);
}

int calculateHumidex(float temp, float humidity) {
  float dewPoint = calculateDewPoint(temp, humidity);
  float e = 6.11 * exp(5417.753 * (1.0/273.16 - 1.0/(273.15 + dewPoint)));
  return (int)(temp + 0.5555 * (e - 10.0));
}

void updateAirQuality() {
  // Read analog value from MQ135
  airQuality.rawADC = analogRead(PIN_AIR_QUALITY_SENSOR);
  
  // Estimate AQI (simple linear mapping)
  airQuality.estimatedAQI = constrain(airQuality.rawADC / 5, 0, 500);
  
  // Determine quality level
  if (airQuality.estimatedAQI <= 50) {
    airQuality.quality = "Good";
  } else if (airQuality.estimatedAQI <= 100) {
    airQuality.quality = "Moderate";
  } else if (airQuality.estimatedAQI <= 150) {
    airQuality.quality = "Unhealthy-S";
  } else if (airQuality.estimatedAQI <= 200) {
    airQuality.quality = "Unhealthy";
  } else if (airQuality.estimatedAQI <= 300) {
    airQuality.quality = "Very Poor";
  } else {
    airQuality.quality = "Hazardous";
  }
  
  airQuality.valid = true;
  airQuality.lastUpdate = millis();
  
  // Update LED display only if value changed significantly
  if (abs(airQuality.rawADC - lastAirQualityValue) > 5) {
    updateAirQualityLEDs();
    lastAirQualityValue = airQuality.rawADC;
  }
}