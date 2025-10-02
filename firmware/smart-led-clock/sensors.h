/**
 * Smart LED Clock - Sensors Module
 * 
 * Handles all sensor operations:
 * - DHT22 temperature/humidity sensors (indoor/outdoor)
 * - MQ135 air quality sensor
 * - Calculations: dew point, humidex, feels-like
 * 
 * Author: F. Baillon
 * Version: Phase 5
 * Date: January 2025
 * License: GPL v3.0
 */

#ifndef SENSORS_H
#define SENSORS_H

#include <DHT.h>
#include "config.h"

// ==========================================
// SENSOR OBJECTS
// ==========================================
extern DHT dhtIndoor;
extern DHT dhtOutdoor;

// ==========================================
// FUNCTION DECLARATIONS
// ==========================================

/**
 * Initialize all sensors
 */
void initSensors();

/**
 * Update temperature and humidity data from DHT22 sensors
 */
void updateSensorData();

/**
 * Update air quality data from MQ135 sensor
 */
void updateAirQuality();

/**
 * Calculate dew point from temperature and humidity
 * @param temp Temperature in Celsius
 * @param humidity Relative humidity in %
 * @return Dew point in Celsius
 */
float calculateDewPoint(float temp, float humidity);

/**
 * Calculate humidex (Canadian humidity index)
 * @param temp Temperature in Celsius
 * @param humidity Relative humidity in %
 * @return Humidex value
 */
int calculateHumidex(float temp, float humidity);

#endif // SENSORS_H