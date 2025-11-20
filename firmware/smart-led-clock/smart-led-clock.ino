/**
 * @file smart-led-clock.ino
 * @brief Smart LED Clock - Main Program
 * 
 * A connected clock combining analog LED display with environmental monitoring.
 * Features include WiFi/NTP sync, dual temperature sensors, air quality monitoring,
 * and multi-mode LCD display with automatic backlight management.
 * 
 * Hardware:
 * - Arduino Uno R4 WiFi
 * - DS3231 RTC module
 * - LCD 20x4 I2C display
 * - 2× DHT22 temperature/humidity sensors
 * - MQ135 air quality sensor
 * - 3× NeoPixel LED strips (12, 60, 10 LEDs)
 * - Push button for mode control
 * - motor
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


#include <Wire.h>

// Include all module headers
#include "config.h"
#include "strings.h"
#include "leds.h"
#include "display.h"
#include "button.h"
#include "rtc.h"
#include "sensors.h"
#include "datalog.h"
#include "storage.h"
#include "webserver.h"
#include "moon.h"


// ==========================================
// WIFI CLIENT (for MQTT)
// ==========================================
WiFiClient mqttWifiClient;

// ==========================================
// GLOBAL VARIABLES (definitions)
// ==========================================
unsigned long lastSecondUpdate = 0;
unsigned long countSensorUpdate = 0;
unsigned short lastSecond = 61;
unsigned short lastMinute = 61;
unsigned short lastHour = 25;

// Wifi attempt to reconnect
int wifiAttempts = 0;

// RTC Interrupt flag (set by ISR in rtc.cpp)
volatile bool secondTicked = false;

// MQTT operation flag (prevent I2C conflicts)
bool mqttBusy = false;

bool isAnimationActive = false;
int animationStep = 0;
int animationHue = 0;

SensorData indoorData = {0, 0, 0, 0, 0, false, 0};
SensorData outdoorData = {0, 0, 0, 0, 0, false, 0};
AirQualityData airQuality = {0, 0, "Unknown", false, 0};

int lastAirQualityValue = -1;

DisplayMode currentDisplayMode = MODE_TEMP_HUMIDITY;
bool lcdBacklightOn = true;
unsigned long lastLCDActivity = 0;

// Runtime configuration variables (can be changed via web)
uint8_t runtimeLedBrightness = 100;
uint32_t runtimeLcdTimeout = LCD_BACKLIGHT_TIMEOUT;
uint8_t runtimeColorHourR = COLOR_HOUR_R;
uint8_t runtimeColorHourG = COLOR_HOUR_G;
uint8_t runtimeColorHourB = COLOR_HOUR_B;
uint8_t runtimeColorMinuteR = COLOR_MINUTE_R;
uint8_t runtimeColorMinuteG = COLOR_MINUTE_G;
uint8_t runtimeColorMinuteB = COLOR_MINUTE_B;
uint8_t runtimeColorSecondR = COLOR_SECOND_R;
uint8_t runtimeColorSecondG = COLOR_SECOND_G;
uint8_t runtimeColorSecondB = COLOR_SECOND_B;
// Runtime NTP & timezone configuration
int8_t runtimeTimezoneOffset = TIME_ZONE_OFFSET;
uint8_t runtimeNtpSyncHour = NTP_SYNC_HOUR;
uint8_t runtimeNtpSyncMinute = NTP_SYNC_MINUTE;


// ==========================================
// SETUP
// ==========================================
void setup() {
  #if DEBUG_MODE
    Serial.begin(115200);
    delay(6000);
    
    Serial.println("=================================");
    Serial.print  ("=== Smart LED Clock v");
    Serial.print(PROJECT_VERSION_STRING);
    Serial.println(" ===");
    Serial.print("Build: ");
    Serial.print(PROJECT_BUILD_DATE);
    Serial.print(" ");
    Serial.println(PROJECT_BUILD_TIME);
    Serial.println("=================================");
    Serial.println();
    Serial.println("Initializing...");
  #endif

  // Initialize I2C
  Wire.begin();
  
  // Initialize LCD
  initDisplay();
  displayStartupMessage(STR_PROJECT_NAME);
  delay(1000);

  // Initialize button
  initButton();
  displayStartupMessage(STR_BUTTON_READY);
  delay(1000);

  // Initialize LED strips
  initLEDs();
  displayStartupMessage(STR_LEDS_READY);
  delay(1000);

  // Initialize sensors
  initSensors();
  displayStartupMessage(STR_SENSORS_READY);
  delay(1000);

  // Initialize DS3231 RTC
  if (!initRTC()) {
#if DEBUG_MODE
  Serial.println("ERROR: RTC initialization failed!");
#endif
    displayStartupMessage(STR_DS3231_ERROR);
    while(1) delay(1000);
  }
  displayStartupMessage(STR_DS3231_READY);
  delay(1000);

  // Initialize Moon Phase Module
  displayStartupMessage(STR_INIT_MOON_PHASE);
  if (initMoon()) {
#if DEBUG_MODE
    Serial.println("Moon module initialized");
#endif  
    displayStartupMessage(STR_MOON_PHASE_READY);
  } else {
#if DEBUG_MODE
    Serial.println("WARNING: Moon module failed");
#endif  
    displayStartupMessage(STR_MOON_PHASE_ERROR);
  }
  delay(2000);

  // Connect to WiFi
  displayStartupMessage(STR_CONNECTING_WIFI);
  if (initWiFi()) {
#if DEBUG_MODE
    Serial.println("WiFi connected");
#endif
    displayStartupMessage(STR_WIFI_CONNECTED);
    delay(1000);
    
    // Synchronize time with NTP
    displayStartupMessage(STR_SYNCING_TIME);
    if (syncTimeWithNTP()) {
#if DEBUG_MODE
  Serial.println("NTP sync successful");
#endif
      displayStartupMessage(STR_TIME_SYNCED);
    } else {
#if DEBUG_MODE
  Serial.println("NTP sync failed");
#endif
      displayStartupMessage(STR_USING_RTC_TIME);
    }

    // Initialize web server
    if (WEB_SERVER_ENABLED) {
      displayStartupMessage(STR_INIT_WEB_SERVER);
      initWebServer();
      delay(1000);
    }

    // Initialize data logging
    if (MQTT_ENABLED) {
      initDataLog(mqttWifiClient);
#if DEBUG_MODE
      Serial.println("Data logging initialized");
#endif
      delay(1000);
    }

    // Initialize EEPROM storage
    displayStartupMessage(STR_LOAD_CONFIG);
    initStorage();
    delay(1000);
  } else {
#if DEBUG_MODE
  Serial.println("WiFi failed");
#endif
    displayStartupMessage(STR_NO_WIFI);
    wifiAttempts = 0;      
  }
  delay(2000);

  // Display current time
  DateTime now = getCurrentTime();
#if DEBUG_MODE
  Serial.print("Current time: ");
#endif
  printDateTime(now);
#if DEBUG_MODE
  Serial.println();
#endif

  // Initial sensor reading
  displayStartupMessage(STR_READING_SENSORS);
  updateSensorData();
  updateAirQuality();
  delay(2000);

  displayStartupMessage(STR_SYSTEM_READY);
  delay(2000);
  
  clearLCD();
  lastLCDActivity = millis();

#if DEBUG_MODE
  Serial.println("System ready!");
  Serial.println();
#endif  
}


// ==========================================
// MAIN LOOP
// ==========================================
void loop() {
  static unsigned long lastAnimationUpdate = 0;
  static DateTime now;

#if DEBUG_MODE
  // Dans loop(), toutes les 30 secondes
  static unsigned long lastMemCheck = 0;
  if (millis() - lastMemCheck >= 30000) {
    lastMemCheck = millis();
    
    // Arduino R4 WiFi - Estimation mémoire via stack pointer
    char top;
    Serial.print("[MEM] Stack pointer: ");
    Serial.print((int)&top, HEX);
    Serial.print(" | Uptime: ");
    Serial.print(millis() / 1000);
    Serial.println("s");
#endif
  }

  // Process button input
  // ====================
  updateButton();

  // Check that wifi connection up
  if (wifiConnected()) {
#if DEBUG_MODE
    if (wifiAttempts > 0) {
      Serial.println("WiFi connected");
    }
#endif    
    // Manage webserver requests
    // =========================
    if (WEB_SERVER_ENABLED)   handleWebServer();

    // Manage data logging
    // ===================
    if (MQTT_ENABLED)   handleDataLog();  

    wifiAttempts = 0;
  }
  else {
    // Try connecting to WiFi
    connectWifi();
  }
  
  // Manage LCD backlight timeout
  // ============================
  manageLCDBacklight();

  // UPDATE CLOCK ON INTERRUPT (when not animating and not MQTT process)
  // The secondTicked flag is set by hardware interrupt (SQW pin)
  // ===================================================================
  if (secondTicked && !isAnimationActive && !mqttBusy) {
    secondTicked = false;  // Reset flag immediately
    
    // Get current time from RTC
    now = getCurrentTime();

#if DEBUG_MODE
    // Validate RTC data
    if (now.year() < 2020 || now.year() > 2100) {
      static int invalidCount = 0;
      Serial.print("RTC INVALID #");
      Serial.print(++invalidCount);
      Serial.print(" - Time: ");
      Serial.print(now.year());
      Serial.print("/");
      Serial.print(now.month());
      Serial.print("/");
      Serial.print(now.day());
      Serial.print(" ");
      Serial.print(now.hour());
      Serial.print(":");
      Serial.print(now.minute());
      Serial.print(":");
      Serial.println(now.second());
    }
#endif

    // Update LED clock display
    // ========================
    updateLEDClock(now);
    
    // Update LCD display (when backlight is on)
    // =========================================
    if (lcdBacklightOn)   updateLCDDisplay(now);

    // Update sensors every 5 seconds
    // ==============================
    if (++countSensorUpdate > SENSOR_UPDATE) {
      countSensorUpdate = 0;
      updateSensorData();
      updateAirQuality();
    }
    
    // Check for hour change to trigger animation
    // ==========================================
    if (now.minute() == 0 && now.second() == 0)   startAnimation();

    // Monthly NTP sync, every 2 days at runtime NTP synchronisation
    // =============================================================
    if (wifiConnected() && now.day() % 2 && now.hour() == runtimeNtpSyncHour && 
        now.minute() == runtimeNtpSyncMinute && now.second() == 0) {
#if DEBUG_MODE
      Serial.println("Monthly NTP sync triggered");
#endif
      syncTimeWithNTP();
    }

    // Update moon position at scheduled time 5:05
    // ===========================================
    if (moonData.isCalibrated && now.hour() == 5 && now.minute() == 5 && now.second() == 0) {
#if DEBUG_MODE
      Serial.println("[MOON] === Scheduled Update ===");
#endif
            
      // Update moon position
      if (updateMoonPosition(now.unixtime())) {
#if DEBUG_MODE
        Serial.print("[MOON] Phase: ");
        Serial.println(getMoonPhaseName(moonData.phase));
#endif
      }
    }

#if DEBUG_MODE
    // Debug output every 10 seconds
    static unsigned short lastDebugSecond = 99;
    if (now.second() % 10 == 0 && now.second() != lastDebugSecond) {
      lastDebugSecond = now.second();
    
      Serial.println(); // Saut de ligne pour séparer
      Serial.print("Time: ");
      printDateTime(now);
      Serial.println();
      Serial.print("Mode: ");
      Serial.print(currentDisplayMode);
      Serial.print(" | LCD: ");
      Serial.print(lcdBacklightOn ? "ON" : "OFF");
      Serial.println();
      Serial.print("Indoor: ");
      Serial.print(indoorData.temperature, 1);
      Serial.print("°C / ");
      Serial.print(indoorData.humidity, 1);
      Serial.println("%");
      Serial.print("Outdoor: ");
      Serial.print(outdoorData.temperature, 1);
      Serial.print("°C / ");
      Serial.print(outdoorData.humidity, 1);
      Serial.println("%");
      Serial.print("Air Quality: ");
      Serial.print(airQuality.estimatedAQI);
      Serial.print(" (");
      Serial.print(airQuality.quality);
      Serial.println(")");

      // Data logging stats
      DataLogStats stats = getLogStats();
      Serial.print("Data: Buffer=");
      Serial.print(stats.bufferCount);
      Serial.print("/");
      Serial.print(MAX_DATA_POINTS);
      Serial.print(" | MQTT=");
      Serial.println(stats.mqttConnected ? "CONNECTED" : "DISCONNECTED");
      Serial.println("---");
    }
#endif    
  }

  // Handle animation
  // ================
  if (isAnimationActive) {
    if (millis() - lastAnimationUpdate >= 50) {
      if (!updateAnimation()) {
        stopAnimation();
      }
      lastAnimationUpdate = millis();
    }
  }

  delay(10);
}