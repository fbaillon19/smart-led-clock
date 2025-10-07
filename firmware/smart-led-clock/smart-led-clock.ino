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
 */

#include <Wire.h>

// Include all module headers
#include "config.h"
#include "strings.h"
#include "leds.h"
#include "sensors.h"
#include "button.h"
#include "rtc.h"
#include "display.h"
#include "webserver.h"

// ==========================================
// GLOBAL VARIABLES (definitions)
// ==========================================
unsigned long lastSecondUpdate = 0;
unsigned long lastLCDUpdate = 0;
unsigned long lastSensorUpdate = 0;
unsigned short lastSecond = 61;
unsigned short lastMinute = 61;
unsigned short lastHour = 25;

bool isAnimationActive = false;
int animationStep = 0;
int animationHue = 0;

SensorData indoorData = {0, 0, 0, 0, 0, false, 0};
SensorData outdoorData = {0, 0, 0, 0, 0, false, 0};
AirQualityData airQuality = {0, 0, "Unknown", false, 0};

int lastAirQualityValue = -1;
bool wifiConnected = false;
bool lastNTPSyncSuccess = false;
unsigned long lastNTPSync = 0;

DisplayMode currentDisplayMode = MODE_TEMP_HUMIDITY;
bool lcdBacklightOn = true;
unsigned long lastLCDActivity = 0;

// ==========================================
// SETUP
// ==========================================
void setup() {
  DEBUG_BEGIN(115200);
  delay(2000);
  
  DEBUG_PRINTLN("=== Smart LED Clock - Phase 5 (Refactored) ===");
  DEBUG_PRINTLN("Initializing...");

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
    DEBUG_PRINTLN("ERROR: RTC initialization failed!");
    displayStartupMessage(STR_DS3231_ERROR);
    while(1) delay(1000);
  }
  displayStartupMessage(STR_DS3231_READY);
  delay(1000);

  // Connect to WiFi
  displayStartupMessage(STR_CONNECTING_WIFI);
  if (connectWiFi()) {
    wifiConnected = true;
    DEBUG_PRINTLN("WiFi connected");
    displayStartupMessage(STR_WIFI_CONNECTED);
    delay(1000);
    
    // Synchronize time with NTP
    displayStartupMessage(STR_SYNCING_TIME);
    if (syncTimeWithNTP()) {
      lastNTPSyncSuccess = true;
      lastNTPSync = millis();
      DEBUG_PRINTLN("NTP sync successful");
      displayStartupMessage(STR_TIME_SYNCED);
    } else {
      DEBUG_PRINTLN("NTP sync failed");
      displayStartupMessage(STR_USING_RTC_TIME);
    }

    // Initialiser le serveur web
    displayStartupMessage(STR_INITIALIZING);
    initWebServer();
    delay(1000);
  } else {
    DEBUG_PRINTLN("WiFi failed");
    displayStartupMessage(STR_NO_WIFI);
  }
  delay(2000);

  // Display current time
  DateTime now = getCurrentTime();
  DEBUG_PRINT("Current time: ");
  printDateTime(now);
  DEBUG_PRINTLN();

  // Initial sensor reading
  displayStartupMessage(STR_READING_SENSORS);
  updateSensorData();
  updateAirQuality();
  delay(2000);

  displayStartupMessage(STR_SYSTEM_READY);
  delay(2000);
  
  clearLCD();
  lastLCDActivity = millis();

  DEBUG_PRINTLN("System ready!");
  DEBUG_PRINTLN();
}

// ==========================================
// MAIN LOOP
// ==========================================
void loop() {
  static unsigned long lastAnimationUpdate = 0;

  // Process button input
  updateButton();

  // Gérer les requêtes web
  if (wifiConnected) {
    handleWebServer();
  }

  // Manage LCD backlight timeout
  manageLCDBacklight();

  // Update clock every second (when not animating)
  if (millis() - lastSecondUpdate >= 1000 && !isAnimationActive) {
    lastSecondUpdate = millis();
    
    DateTime now = getCurrentTime();
    updateLEDClock(now);
    
    // Check for hour change to trigger animation
    if (now.minute() == 0 && now.second() == 0 && !isAnimationActive) {
      startAnimation();
    }

    // Daily NTP sync
    if (wifiConnected && now.hour() == NTP_SYNC_HOUR && 
        now.minute() == NTP_SYNC_MINUTE && now.second() == 0) {
      DEBUG_PRINTLN("Daily NTP sync triggered");
      if (syncTimeWithNTP()) {
        lastNTPSync = millis();
        lastNTPSyncSuccess = true;
      }
    }

    // Debug output every 10 seconds
    if (now.second() % 10 == 0) {
      DEBUG_PRINT("Time: ");
      printDateTime(now);
      DEBUG_PRINT(" | Mode: ");
      DEBUG_PRINT(currentDisplayMode);
      DEBUG_PRINT(" | LCD: ");
      DEBUG_PRINT(lcdBacklightOn ? "ON" : "OFF");
      DEBUG_PRINT(" | Indoor: ");
      DEBUG_PRINT(indoorData.temperature, 1);
      DEBUG_PRINT("°C | Outdoor: ");
      DEBUG_PRINT(outdoorData.temperature, 1);
      DEBUG_PRINT("°C | AQI: ");
      DEBUG_PRINTLN(airQuality.estimatedAQI);
    }
  }

  // Update sensors every 2 seconds
  if (millis() - lastSensorUpdate >= SENSOR_READ_INTERVAL) {
    lastSensorUpdate = millis();
    updateSensorData();
    updateAirQuality();
  }

  // Update LCD display every 2 seconds (when not animating and backlight is on)
  if (millis() - lastLCDUpdate >= 2000 && !isAnimationActive && lcdBacklightOn) {
    lastLCDUpdate = millis();
    DateTime now = getCurrentTime();
    updateLCDDisplay(now);
  }

  // Handle animation
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