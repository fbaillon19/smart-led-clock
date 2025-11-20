/**
 * @file sensors.h
 * @brief Environmental sensors module
 * 
 * Manages all environmental sensing operations including temperature,
 * humidity, and air quality monitoring.
 * 
 * Sensors supported:
 * - DHT22: Temperature and humidity (indoor/outdoor)
 * - MQ135: Air quality sensor (VOCs, CO2, NH3)
 * 
 * Calculated metrics:
 * - Heat index (feels-like temperature)
 * - Dew point
 * - Humidex (Canadian humidity comfort index)
 * - Air Quality Index (AQI) estimation
 * 
 * @author F. Baillon
 * @version 1.1.0
 * @date November 2025
 * @license MIT License
 * 
 * Copyright (c) 2025 F. Baillon
 */

#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <DHT.h>
#include "config.h"
#include "strings.h"
#include "leds.h"


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