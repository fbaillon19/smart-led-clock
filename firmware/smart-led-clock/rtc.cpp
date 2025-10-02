/**
 * Smart LED Clock - RTC Module Implementation
 * 
 * Author: F. Baillon
 * Version: Phase 5
 * Date: January 2025
 * License: GPL v3.0
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

bool connectWiFi() {
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

bool syncTimeWithNTP() {
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

void printDateTime(DateTime dt) {
  char buffer[20];
  sprintf(buffer, "%04d/%02d/%02d %02d:%02d:%02d", 
          dt.year(), dt.month(), dt.day(),
          dt.hour(), dt.minute(), dt.second());
  Serial.print(buffer);
}

DateTime getCurrentTime() {
  return rtc.now();
}