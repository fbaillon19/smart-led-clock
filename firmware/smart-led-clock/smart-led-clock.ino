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
#include "rtc.h"
#include "sensors.h"
#include "display.h"
#include "leds.h"
#include "button.h"

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
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== Smart LED Clock - Phase 5 (Refactored) ===");
  Serial.println("Initializing...");

  // Initialize I2C
  Wire.begin();
  
  // Initialize LCD
  initDisplay();
  displayStartupMessage("Initializing...");
  delay(1000);

  // Initialize button
  initButton();
  displayStartupMessage("Button Ready");
  delay(1000);

  // Initialize LED strips
  initLEDs();
  displayStartupMessage("LEDs Ready");
  delay(1000);

  // Initialize sensors
  initSensors();
  displayStartupMessage("Sensors Ready");
  delay(1000);

  // Initialize DS3231 RTC
  if (!initRTC()) {
    Serial.println("ERROR: RTC initialization failed!");
    displayStartupMessage("DS3231 ERROR!");
    while(1) delay(1000);
  }
  displayStartupMessage("DS3231 Ready");
  delay(1000);

  // Connect to WiFi
  displayStartupMessage("Connecting WiFi...");
  if (connectWiFi()) {
    wifiConnected = true;
    Serial.println("WiFi connected");
    displayStartupMessage("WiFi Connected");
    delay(1000);
    
    // Synchronize time with NTP
    displayStartupMessage("Syncing time...");
    if (syncTimeWithNTP()) {
      lastNTPSyncSuccess = true;
      lastNTPSync = millis();
      Serial.println("NTP sync successful");
      displayStartupMessage("Time Synchronized");
    } else {
      Serial.println("NTP sync failed");
      displayStartupMessage("Using RTC time");
    }
  } else {
    Serial.println("WiFi failed");
    displayStartupMessage("No WiFi - Using RTC");
  }
  delay(2000);

  // Display current time
  DateTime now = getCurrentTime();
  Serial.print("Current time: ");
  printDateTime(now);
  Serial.println();

  // Initial sensor reading
  displayStartupMessage("Reading sensors...");
  updateSensorData();
  updateAirQuality();
  delay(2000);

  displayStartupMessage("System Ready!");
  delay(2000);
  
  clearLCD();
  lastLCDActivity = millis();

  Serial.println("System ready!");
  Serial.println();
}

// ==========================================
// MAIN LOOP
// ==========================================
void loop() {
  static unsigned long lastAnimationUpdate = 0;

  // Process button input
  updateButton();

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
      Serial.println("Daily NTP sync triggered");
      if (syncTimeWithNTP()) {
        lastNTPSync = millis();
        lastNTPSyncSuccess = true;
      }
    }

    // Debug output every 10 seconds
    if (now.second() % 10 == 0) {
      Serial.print("Time: ");
      printDateTime(now);
      Serial.print(" | Mode: ");
      Serial.print(currentDisplayMode);
      Serial.print(" | LCD: ");
      Serial.print(lcdBacklightOn ? "ON" : "OFF");
      Serial.print(" | Indoor: ");
      Serial.print(indoorData.temperature, 1);
      Serial.print("°C | Outdoor: ");
      Serial.print(outdoorData.temperature, 1);
      Serial.print("°C | AQI: ");
      Serial.println(airQuality.estimatedAQI);
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