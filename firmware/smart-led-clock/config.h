/**
 * @file config.h
 * @brief Configuration header for Smart LED Clock
 * 
 * Centralizes all configuration constants, pin definitions, and data structures.
 * This file should be the first to include when modifying hardware connections
 * or adjusting system parameters.
 * 
 * @author F. Baillon
 * @version 1.0.0
 * @date January 2025
 * @license MIT License
 * 
 * Copyright (c) 2025 F. Baillon
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==========================================
// PIN DEFINITIONS
// ==========================================
#define PIN_LEDS_MINUTE_SECOND  9
#define PIN_LEDS_HOUR          10
#define PIN_LEDS_AIR_QUALITY   11
#define PIN_DHT_INDOOR          5
#define PIN_DHT_OUTDOOR         6
#define PIN_AIR_QUALITY_SENSOR  A0
#define PIN_BUTTON              2

// ==========================================
// SENSOR CONFIGURATION
// ==========================================
#define DHT_TYPE                DHT22
#define SENSOR_READ_INTERVAL    2000  // Read sensors every 2 seconds

// ==========================================
// NTP & TIME CONFIGURATION
// ==========================================
#define NTP_SYNC_HOUR           1     // Daily sync at 1:01 AM
#define NTP_SYNC_MINUTE         1
#define TIME_ZONE_OFFSET        2     // UTC+2

// ==========================================
// LCD CONFIGURATION
// ==========================================
#define LCD_I2C_ADDRESS         0x27
#define LCD_COLUMNS             20
#define LCD_ROWS                4
#define LCD_BACKLIGHT_TIMEOUT   30000  // 30 seconds

// ==========================================
// LED CONFIGURATION
// ==========================================
#define NUM_LEDS_HOUR           12
#define NUM_LEDS_MINUTE_SECOND  60
#define NUM_LEDS_AIR_QUALITY    10

// LED Colors
#define COLOR_SECOND_R          0
#define COLOR_SECOND_G          127
#define COLOR_SECOND_B          0

#define COLOR_MINUTE_R          127
#define COLOR_MINUTE_G          0
#define COLOR_MINUTE_B          0

#define COLOR_OVERLAP_R         127
#define COLOR_OVERLAP_G         127
#define COLOR_OVERLAP_B         0

#define COLOR_HOUR_R            0
#define COLOR_HOUR_G            0
#define COLOR_HOUR_B            127

// ==========================================
// DISPLAY MODES
// ==========================================
/**
 * @enum DisplayMode
 * @brief Available LCD display modes
 * 
 * Users can cycle through these modes using the button:
 * - MODE_TEMP_HUMIDITY: Shows indoor/outdoor temperature and humidity
 * - MODE_FEELS_LIKE: Shows feels-like temperature and dew point
 * - MODE_HUMIDEX: Shows Canadian humidity comfort index
 */
enum DisplayMode {
  MODE_TEMP_HUMIDITY = 0,  ///< Temperature & Humidity (default)
  MODE_FEELS_LIKE = 1,     ///< Feels-like & Dew point
  MODE_HUMIDEX = 2,        ///< Humidex comfort index
  MODE_COUNT = 3           ///< Total number of modes
};

// ==========================================
// SENSOR DATA STRUCTURES
// ==========================================
/**
 * @struct SensorData
 * @brief Temperature and humidity sensor data container
 */
struct SensorData {
  float temperature;        ///< Temperature in Celsius
  float humidity;          ///< Relative humidity in %
  float feelsLike;         ///< Feels-like temperature in Celsius
  float dewPoint;          ///< Dew point in Celsius
  int humidex;            ///< Canadian humidex index
  bool valid;             ///< Data validity flag
  unsigned long lastUpdate; ///< Timestamp of last update (millis)
};

/**
 * @struct AirQualityData
 * @brief Air quality sensor data container
 */
struct AirQualityData {
  int rawADC;              ///< Raw ADC value from MQ135
  int estimatedAQI;        ///< Estimated Air Quality Index (0-500)
  const char* quality;     ///< Quality description string
  bool valid;             ///< Data validity flag
  unsigned long lastUpdate; ///< Timestamp of last update (millis)
};

// ==========================================
// GLOBAL VARIABLES (extern declarations)
// ==========================================
// Timing variables
extern unsigned long lastSecondUpdate;
extern unsigned long lastLCDUpdate;
extern unsigned long lastSensorUpdate;

// LED tracking
extern unsigned short lastSecond;
extern unsigned short lastMinute;
extern unsigned short lastHour;

// Animation state
extern bool isAnimationActive;
extern int animationStep;
extern int animationHue;

// Sensor data
extern SensorData indoorData;
extern SensorData outdoorData;
extern AirQualityData airQuality;
extern int lastAirQualityValue;

// WiFi & NTP state
extern bool wifiConnected;
extern bool lastNTPSyncSuccess;
extern unsigned long lastNTPSync;

// Display state
extern DisplayMode currentDisplayMode;
extern bool lcdBacklightOn;
extern unsigned long lastLCDActivity;

#endif // CONFIG_H