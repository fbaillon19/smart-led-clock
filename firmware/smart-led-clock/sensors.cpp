/**
 * @file sensors.cpp
 * @brief Environmental sensors implementation
 * 
 * @author F. Baillon
 * @version 1.0.0
 * @date January 2025
 * @license MIT License
 * 
 * Copyright (c) 2025 F. Baillon
 */

#include "sensors.h"

// ==========================================
// GLOBAL SENSOR OBJECTS
// ==========================================
DHT dhtIndoor(PIN_DHT_INDOOR, DHT_TYPE);
DHT dhtOutdoor(PIN_DHT_OUTDOOR, DHT_TYPE);

// ==========================================
// FUNCTION IMPLEMENTATIONS
// ==========================================

/**
 * @brief Initialize all environmental sensors
 * 
 * Initializes DHT22 sensors and MQ135 air quality sensor.
 * Sets up pin modes and prepares sensors for reading.
 */
void initSensors() {
  // Initialize DHT22 sensors
  dhtIndoor.begin();
  dhtOutdoor.begin();
  DEBUG_PRINTLN("DHT22 sensors initialized");
  
  // Initialize air quality sensor
  pinMode(PIN_AIR_QUALITY_SENSOR, INPUT);
  DEBUG_PRINTLN("MQ135 air quality sensor initialized");
}

/**
 * @brief Update temperature and humidity data from DHT22 sensors
 * 
 * Reads both indoor and outdoor DHT22 sensors. For each sensor:
 * - Reads raw temperature and humidity
 * - Validates readings (checks for NaN)
 * - Calculates derived values: feels-like, dew point, humidex
 * - Updates global sensor data structures
 * - Sets validity flag and timestamp
 * 
 * If a sensor read fails, its valid flag is set to false and
 * an error message is printed to Serial.
 * 
 * Call frequency: Every 2 seconds (SENSOR_READ_INTERVAL)
 */
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
    DEBUG_PRINTLN("ERROR: Indoor sensor read failed");
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
    DEBUG_PRINTLN("ERROR: Outdoor sensor read failed");
  }
}

/**
 * @brief Calculate dew point from temperature and humidity
 * 
 * Uses the Magnus-Tetens approximation formula to calculate
 * the dew point temperature. The dew point is the temperature
 * at which air becomes saturated and water vapor condenses.
 * 
 * Formula: Td = (b * α) / (a - α)
 * where α = ln(RH/100) + (a*T)/(b+T)
 * 
 * @param temp Temperature in Celsius
 * @param humidity Relative humidity in percentage (0-100)
 * @return Dew point in Celsius
 */
float calculateDewPoint(float temp, float humidity) {
  float alpha = log(humidity / 100.0) + (17.27 * temp) / (237.3 + temp);
  return (237.3 * alpha) / (17.27 - alpha);
}

/**
 * @brief Calculate Canadian Humidex comfort index
 * 
 * The humidex (humidity index) is a Canadian innovation that
 * combines temperature and humidity into one number to reflect
 * the perceived temperature. It's similar to the heat index.
 * 
 * Humidex scale:
 * - < 20: No discomfort
 * - 20-29: Little discomfort
 * - 30-39: Some discomfort
 * - 40-45: Great discomfort, avoid exertion
 * - > 45: Dangerous, heat stroke possible
 * 
 * @param temp Temperature in Celsius
 * @param humidity Relative humidity in percentage (0-100)
 * @return Humidex value (unitless index)
 */
int calculateHumidex(float temp, float humidity) {
  float dewPoint = calculateDewPoint(temp, humidity);
  float e = 6.11 * exp(5417.753 * (1.0/273.16 - 1.0/(273.15 + dewPoint)));
  return (int)(temp + 0.5555 * (e - 10.0));
}

/**
 * @brief Update air quality data from MQ135 sensor
 * 
 * Reads the MQ135 analog sensor and estimates Air Quality Index (AQI).
 * 
 * Process:
 * 1. Read raw ADC value (0-1023)
 * 2. Convert to estimated AQI (0-500 scale)
 * 3. Determine quality category (Good/Moderate/Unhealthy/etc.)
 * 4. Update LED bar if value changed significantly (>5 ADC units)
 * 
 * AQI Categories:
 * - 0-50: Good (green)
 * - 51-100: Moderate (yellow)
 * - 101-150: Unhealthy for sensitive groups (orange)
 * - 151-200: Unhealthy (red)
 * - 201-300: Very unhealthy (purple)
 * - 301+: Hazardous (maroon)
 * 
 * Note: MQ135 requires 24-48h warm-up for accurate readings.
 * The ADC to AQI conversion is a simple linear approximation
 * and should be calibrated for your specific environment.
 */
void updateAirQuality() {
  // Read analog value from MQ135
  airQuality.rawADC = analogRead(PIN_AIR_QUALITY_SENSOR);
  
  // Estimate AQI (simple linear mapping)
  // TODO: Calibrate this conversion for accurate readings
  airQuality.estimatedAQI = constrain(airQuality.rawADC / 5, 0, 500);
  
  // Determine quality level
  if (airQuality.estimatedAQI <= 50) {
    airQuality.quality = STR_AIR_QA_GOOD;
  } else if (airQuality.estimatedAQI <= 100) {
    airQuality.quality = STR_AIR_QA_MODERATE;
  } else if (airQuality.estimatedAQI <= 150) {
    airQuality.quality = STR_AIR_QA_UNHEALTHY_SG;
  } else if (airQuality.estimatedAQI <= 200) {
    airQuality.quality = STR_AIR_QA_UNHEALTHY;
  } else if (airQuality.estimatedAQI <= 300) {
    airQuality.quality = STR_AIR_QA_VERY_POOR;
  } else {
    airQuality.quality = STR_AIR_QA_HAZARDOUS;
  }
  
  airQuality.valid = true;
  airQuality.lastUpdate = millis();
  
  // Update LED display only if value changed significantly
  // Prevents excessive LED updates for minor fluctuations
  if (abs(airQuality.rawADC - lastAirQualityValue) > 5) {
    updateAirQualityLEDs();
    lastAirQualityValue = airQuality.rawADC;
  }
}