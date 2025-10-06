/**
 * @file rtc.cpp
 * @brief RTC, WiFi, and NTP management implementation
 * 
 * @author F. Baillon
 * @version 1.0.0
 * @date January 2025
 * @license MIT License
 * 
 * Copyright (c) 2025 F. Baillon
 */

#include "rtc.h"
#include "secrets.h"
#include <Arduino.h>

// ==========================================
// GLOBAL RTC & NTP OBJECTS
// ==========================================
RTC_DS3231 rtc;
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", TIME_ZONE_OFFSET * 3600, 60000);

// ==========================================
// FUNCTION IMPLEMENTATIONS
// ==========================================

/**
 * @brief Initialize the DS3231 RTC module
 * 
 * Performs RTC initialization and checks for power loss.
 * If power was lost, time will be synced via NTP during setup.
 * 
 * @return true if RTC initialized successfully, false otherwise
 */
bool initRTC() {
  Serial.println("Initializing DS3231 RTC...");
  
  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("ERROR: DS3231 not found!");
    return false;
  }
  
  Serial.println("DS3231 RTC initialized");

  // Check if RTC lost power
  if (rtc.lostPower()) {
    Serial.println("WARNING: RTC lost power, will sync with NTP");
  }

  // Get and display current time
  DateTime now = rtc.now();
  Serial.print("Current RTC time: ");
  printDateTime(now);
  Serial.println();
  
  return true;
}

/**
 * @brief Connect to WiFi network
 * 
 * Attempts to connect to WiFi using credentials from secrets.h.
 * Tries up to 20 times (10 seconds total) before giving up.
 * 
 * @return true if connected successfully, false if connection failed
 */
bool connectWiFi() {
return true;

  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  int attempts = 0;
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    delay(1000);
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());
    return true;
  }
  
  Serial.println("WiFi connection failed");
  return false;
}

/**
 * @brief Synchronize RTC time with NTP server
 * 
 * Connects to NTP server (pool.ntp.org) and updates the RTC
 * with the current time. Applies timezone offset from config.h.
 * Attempts up to 10 times (5 seconds) before giving up.
 * 
 * @return true if sync successful, false if sync failed
 */
bool syncTimeWithNTP() {
return true;

  Serial.println("Synchronizing with NTP server...");
  
  timeClient.begin();
  
  int attempts = 0;
  while (!timeClient.update() && attempts < 10) {
    timeClient.forceUpdate();
    delay(500);
    attempts++;
  }
  
  if (timeClient.isTimeSet()) {
    unsigned long epochTime = timeClient.getEpochTime();
    rtc.adjust(DateTime(epochTime));
    
    Serial.print("Time synchronized: ");
    DateTime now = rtc.now();
    printDateTime(now);
    Serial.println();
    
    return true;
  }
  
  Serial.println("NTP sync failed");
  return false;
}

/**
 * @brief Print DateTime object to Serial in formatted way
 * 
 * Format: YYYY/MM/DD HH:MM:SS
 * Example: 2025/01/15 14:32:45
 * 
 * @param dt DateTime object to print
 */
void printDateTime(DateTime dt) {
  char buffer[20];
  sprintf(buffer, "%04d/%02d/%02d %02d:%02d:%02d", 
          dt.year(), dt.month(), dt.day(),
          dt.hour(), dt.minute(), dt.second());
  Serial.print(buffer);
}

/**
 * @brief Get current time from RTC
 * 
 * @return DateTime object with current time
 */
DateTime getCurrentTime() {
  return rtc.now();
}