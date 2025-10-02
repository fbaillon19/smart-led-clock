/**
 * Smart LED Clock - Phase 5: Simplified Button Control
 * Main File (Refactored)
 * 
 * Single button control with LCD backlight management:
 * 
 * Button behavior:
 * - Click when LCD OFF: Wake LCD (no mode change)
 * - Click when LCD ON: Cycle through display modes
 * - Long press: Return to default mode
 * 
 * Display modes:
 * 1. Temperature & Humidity (default)
 * 2. Feels-like & Dew point
 * 3. Humidex
 * 
 * LCD backlight:
 * - Auto-off after 30 seconds of inactivity
 * - Wakes on button press
 * 
 * Components:
 * - DS3231 RTC module
 * - 12 LED ring (hours)
 * - 60 LED ring (minutes/seconds)
 * - 10 LED bar (air quality indicator)
 * - 20x4 I2C LCD display
 * - 2x DHT22 sensors (indoor/outdoor)
 * - MQ135 air quality sensor
 * - Push button (mode control)
 * - WiFi (Arduino Uno R4 WiFi)
 * - NTP time synchronization
 * 
 * Author: F. Baillon
 * Version: Phase 5
 * Date: January 2025
 * License: GPL v3.0
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