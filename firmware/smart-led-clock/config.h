/**
 * Smart LED Clock - Configuration Header
 * 
 * Centralizes all configuration constants, pin definitions,
 * and data structures used throughout the project.
 * 
 * Author: F. Baillon
 * Version: Phase 5
 * Date: January 2025
 * License: GPL v3.0
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
enum DisplayMode {
  MODE_TEMP_HUMIDITY = 0,  // Default: Temperature & Humidity
  MODE_FEELS_LIKE = 1,     // Feels-like & Dew point
  MODE_HUMIDEX = 2,        // Humidex index
  MODE_COUNT = 3           // Total number of modes
};

// ==========================================
// SENSOR DATA STRUCTURES
// ==========================================
struct SensorData {
  float temperature;
  float humidity;
  float feelsLike;
  float dewPoint;
  int humidex;
  bool valid;
  unsigned long lastUpdate;
};

struct AirQualityData {
  int rawADC;
  int estimatedAQI;
  const char* quality;
  bool valid;
  unsigned long lastUpdate;
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