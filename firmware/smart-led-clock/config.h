/**
 * Smart LED Clock - Hardware Configuration
 * 
 * This file contains all hardware pin definitions and general configuration
 * settings for the Smart LED Clock project.
 * 
 * Author: F. Baillon
 * Version: 1.0
 * License: MIT
 */

#ifndef CONFIG_H
#define CONFIG_H

// ==========================================
// PIN DEFINITIONS
// ==========================================

// LED Strips
#define PIN_LEDS_MINUTE_SECOND  9     // 60 LEDs ring for minutes/seconds (PWM pin D9)
#define PIN_LEDS_HOUR          10     // 12 LEDs ring for hours (PWM pin D10)
#define PIN_LEDS_AIR_QUALITY   11     // 10 LEDs bar for air quality (PWM pin D11)

// Temperature Sensors (DHT22)
#define PIN_DHT_INDOOR          5     // Indoor temperature sensor (Digital pin D5)
#define PIN_DHT_OUTDOOR         6     // Outdoor temperature sensor (Digital pin D6)

// User Interface
#define PIN_BUTTON_1            2     // Display mode button (Digital pin D2)
#define PIN_BUTTON_2            3     // Secondary button (Digital pin D3)

// Air Quality Sensor
#define PIN_AIR_QUALITY_SENSOR  A0    // MQ135 air quality sensor (Analog pin A0)

// ==========================================
// LED CONFIGURATION
// ==========================================

#define NUM_LEDS_HOUR           12    // Number of LEDs in hour ring
#define NUM_LEDS_MINUTE_SECOND  60    // Number of LEDs in minute/second ring
#define NUM_LEDS_AIR_QUALITY    10    // Number of LEDs in air quality bar

// LED Colors (RGB values 0-255)
#define COLOR_SECOND_R          0     // Second hand color - Green
#define COLOR_SECOND_G          127
#define COLOR_SECOND_B          0

#define COLOR_MINUTE_R          127   // Minute hand color - Red
#define COLOR_MINUTE_G          0
#define COLOR_MINUTE_B          0

#define COLOR_OVERLAP_R         127   // When second and minute overlap - Yellow
#define COLOR_OVERLAP_G         127
#define COLOR_OVERLAP_B         0

#define COLOR_HOUR_R            0     // Hour hand color - Blue
#define COLOR_HOUR_G            0
#define COLOR_HOUR_B            127

// ==========================================
// I2C DEVICES CONFIGURATION
// ==========================================

#define LCD_I2C_ADDRESS         0x27  // I2C address of LCD display
#define RTC_I2C_ADDRESS         0x68  // I2C address of DS3231 RTC module

// LCD Configuration
#define LCD_COLUMNS             20    // Number of characters per line
#define LCD_ROWS                4     // Number of lines

// ==========================================
// SENSOR CONFIGURATION
// ==========================================

#define DHT_TYPE                DHT22 // DHT sensor type
#define SENSOR_READ_INTERVAL    2000  // Sensor reading interval (milliseconds)

// Air Quality Index (AQI) thresholds
#define AQI_GOOD                50    // Good air quality threshold
#define AQI_MODERATE            100   // Moderate air quality threshold
#define AQI_UNHEALTHY_SENSITIVE 150   // Unhealthy for sensitive groups
#define AQI_UNHEALTHY           200   // Unhealthy threshold
#define AQI_VERY_UNHEALTHY      300   // Very unhealthy threshold
#define AQI_HAZARDOUS           500   // Hazardous threshold

// ==========================================
// TIMING CONFIGURATION
// ==========================================

#define NTP_SYNC_HOUR           1     // Hour for daily NTP synchronization (1 AM)
#define NTP_SYNC_MINUTE         1     // Minute for daily NTP synchronization
#define TIME_ZONE_OFFSET        1     // UTC offset for Paris (UTC+1)

#define ANIMATION_DURATION      5000  // Hourly animation duration (milliseconds)
#define DISPLAY_TIMEOUT         4000  // Temporary display timeout (milliseconds)

// ==========================================
// DISPLAY MODES (Bit flags)
// ==========================================

#define DISPLAY_TEMP_HUMIDITY   0x01  // Display temperature and humidity
#define DISPLAY_FEELS_LIKE      0x02  // Display feels-like temperature and dew point
#define DISPLAY_PERMANENT_FEELS 0x04  // Permanent feels-like display mode
#define DISPLAY_HUMIDEX         0x08  // Display humidex values
#define DISPLAY_TEMPORARY       0x10  // Temporary display mode active
#define DISPLAY_DATE_TIME       0x20  // Display date and time on LCD

// ==========================================
// WEB SERVER CONFIGURATION
// ==========================================

#define WEB_SERVER_PORT         80    // Web interface port
#define AP_MODE_TIMEOUT         300   // Access Point mode timeout (seconds)

// ==========================================
// DEBUG CONFIGURATION
// ==========================================

#define ENABLE_SERIAL_DEBUG     true  // Enable serial debug output
#define SERIAL_BAUD_RATE        115200 // Serial communication baud rate

#endif // CONFIG_H