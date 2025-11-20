/**
 * @file config.h
 * @brief Configuration header for Smart LED Clock
 * 
 * Centralizes all configuration constants, pin definitions, and data structures.
 * This file should be the first to include when modifying hardware connections
 * or adjusting system parameters.
 * 
 * @author F. Baillon
 * @version 1.1.0
 * @date November 2025
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
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==========================================
// PROJECT VERSION
// ==========================================
#define PROJECT_VERSION_MAJOR 1
#define PROJECT_VERSION_MINOR 2
#define PROJECT_VERSION_PATCH 0
#define PROJECT_VERSION_STRING "1.2.0"
#define PROJECT_BUILD_DATE __DATE__
#define PROJECT_BUILD_TIME __TIME__

// ==========================================
// DEBUG CONFIGURATION
// ==========================================
/**
 * Debug mode: Set to 1 to enable serial debug output, 0 to disable
 * When disabled, all DEBUG_PRINT macros become no-ops (zero overhead)
 */
#define DEBUG_MODE 1

// Debug macros - only active when DEBUG_MODE is enabled
#if DEBUG_MODE
  #define DEBUG_PRINT(x)       Serial.print(x)
  #define DEBUG_PRINTLN(x)     Serial.println(x)
  #define DEBUG_PRINTF(x, y)   Serial.printf(x, y)
  #define DEBUG_BEGIN(x)       Serial.begin(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(x, y)
  #define DEBUG_BEGIN(x)
#endif

// ==========================================
// LANGUAGE CONFIGURATION
// ==========================================
/**
 * Language selection for LCD display
 * Supported languages:
 * - LANG_FR: French (default)
 * - LANG_EN: English
 */
#define LANG_FR 0
#define LANG_EN 1

// Set your preferred language here
#define LANGUAGE LANG_FR

// ==========================================
// PIN DEFINITIONS
// ==========================================
#define PIN_LEDS_MINUTE_SECOND  9    ///< NeoPixel minute/second ring (60 LEDs)
#define PIN_LEDS_HOUR          10    ///< NeoPixel hour ring (12 LEDs)
#define PIN_LEDS_AIR_QUALITY   11    ///< NeoPixel air quality bar (10 LEDs)
#define PIN_DHT_INDOOR          5    ///< DHT22 indoor sensor data pin
#define PIN_DHT_OUTDOOR         6    ///< DHT22 outdoor sensor data pin
#define PIN_AIR_QUALITY_SENSOR  A0   ///< MQ135 analog input
#define PIN_BUTTON              13   ///< Push button input (with pull-up)
#define PIN_DS3231_SQW          2    ///< DS3231 Square Wave interrupt output (1Hz)

// Moon Phase Module (NEW)
#define PIN_MOON_STEPPER_IN1    3    // ULN2003 IN1
#define PIN_MOON_STEPPER_IN2    4    // ULN2003 IN2
#define PIN_MOON_STEPPER_IN3    7    // ULN2003 IN3
#define PIN_MOON_STEPPER_IN4    8    // ULN2003 IN4
#define PIN_MOON_LDR_SENSOR     A1   // LDR photoresistor (analog)
#define PIN_MOON_CALIB_LED      12   // LED indicator for calibration

// ==========================================
// WIFI CONFIGURATION
// ==========================================
// Activer/désactiver Web Server temporairement
#define WEB_SERVER_ENABLED      true    // Set to true to activate and to false to deactivate.
#define MAX_WIFI_ATTEMPTS       300     // Wait for 3 seconds

// ==========================================
// SENSOR CONFIGURATION
// ==========================================
#define DHT_TYPE                DHT22
#define SENSOR_UPDATE           5       ///< Sensor read every 5 seconds

// ==========================================
// DATA LOGGING CONFIGURATION
// ==========================================
// Activer/désactiver MQTT temporairement
#define MQTT_ENABLED            true    // Set to true to activate and to false to deactivate.

// Réduire les timeouts
#define MQTT_CONNECT_TIMEOUT    2000    // 2 secondes
#define MQTT_SOCKET_TIMEOUT     2       // 2 secondes
#define MQTT_RETRY_INTERVAL     30000   // Essayer toutes les 30s au lieu de 10min

// ==========================================
// NTP & TIME CONFIGURATION
// ==========================================
#define NTP_SYNC_HOUR           1     ///< Daily NTP sync hour (0-23)
#define NTP_SYNC_MINUTE         1     ///< Daily NTP sync minute (0-59)
#define TIME_ZONE_OFFSET        1     ///< UTC offset in hours (e.g., 2 for UTC+2)

// ==========================================
// LCD CONFIGURATION
// ==========================================
#define LCD_I2C_ADDRESS         0x27  ///< I2C address of LCD display
#define LCD_COLUMNS             20    ///< Number of LCD columns
#define LCD_ROWS                4     ///< Number of LCD rows
#define LCD_BACKLIGHT_TIMEOUT   30000 ///< Auto-off timeout in milliseconds (30s)

// ==========================================
// LED CONFIGURATION
// ==========================================
#define NUM_LEDS_HOUR           12    ///< Number of LEDs in hour ring
#define NUM_LEDS_MINUTE_SECOND  60    ///< Number of LEDs in minute/second ring
#define NUM_LEDS_AIR_QUALITY    10    ///< Number of LEDs in air quality bar

// LED Colors (RGB values 0-255)
#define COLOR_SECOND_R          0     ///< Second hand red component
#define COLOR_SECOND_G          127   ///< Second hand green component
#define COLOR_SECOND_B          0     ///< Second hand blue component

#define COLOR_MINUTE_R          127   ///< Minute hand red component
#define COLOR_MINUTE_G          0     ///< Minute hand green component
#define COLOR_MINUTE_B          0     ///< Minute hand blue component

#define COLOR_OVERLAP_R         127   ///< Overlap color red component
#define COLOR_OVERLAP_G         127   ///< Overlap color green component
#define COLOR_OVERLAP_B         0     ///< Overlap color blue component

#define COLOR_HOUR_R            0     ///< Hour hand red component
#define COLOR_HOUR_G            0     ///< Hour hand green component
#define COLOR_HOUR_B            127   ///< Hour hand blue component

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

// RTC Interrupt (SQW) - for precise 1Hz timing
extern volatile bool secondTicked;      ///< Flag set by interrupt every second

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
extern bool lastNTPSyncSuccess;
extern unsigned long lastNTPSync;

// Display state
extern DisplayMode currentDisplayMode;
extern bool lcdBacklightOn;
extern unsigned long lastLCDActivity;

// ==========================================
// RUNTIME CONFIGURATION VARIABLES
// ==========================================
// These can be modified at runtime via web interface
extern uint8_t runtimeLedBrightness;
extern uint32_t runtimeLcdTimeout;
extern uint8_t runtimeColorHourR;
extern uint8_t runtimeColorHourG;
extern uint8_t runtimeColorHourB;
extern uint8_t runtimeColorMinuteR;
extern uint8_t runtimeColorMinuteG;
extern uint8_t runtimeColorMinuteB;
extern uint8_t runtimeColorSecondR;
extern uint8_t runtimeColorSecondG;
extern uint8_t runtimeColorSecondB;
extern int8_t runtimeTimezoneOffset;
extern uint8_t runtimeNtpSyncHour;
extern uint8_t runtimeNtpSyncMinute;

#endif // CONFIG_H