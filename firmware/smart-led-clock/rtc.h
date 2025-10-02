/**
 * Smart LED Clock - RTC Module
 * 
 * Handles all DS3231 RTC operations including:
 * - Initialization and power loss detection
 * - Time reading and adjustment
 * - WiFi connection management
 * - NTP synchronization
 * - DateTime formatting utilities
 * 
 * Author: F. Baillon
 * Version: Phase 5
 * Date: January 2025
 * License: GPL v3.0
 */

#ifndef RTC_H
#define RTC_H

#include <RTClib.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <LiquidCrystal_I2C.h>
#include "config.h"

// ==========================================
// RTC & NTP OBJECTS
// ==========================================
extern RTC_DS3231 rtc;
extern WiFiUDP udp;
extern NTPClient timeClient;

// ==========================================
// FUNCTION DECLARATIONS
// ==========================================

/**
 * Initialize the DS3231 RTC module
 * @return true if successful, false otherwise
 */
bool initRTC();

/**
 * Connect to WiFi network
 * @return true if connected, false otherwise
 */
bool connectWiFi();

/**
 * Synchronize RTC time with NTP server
 * @return true if successful, false otherwise
 */
bool syncTimeWithNTP();

/**
 * Print DateTime object to Serial in formatted way
 * Format: YYYY/MM/DD HH:MM:SS
 * @param dt DateTime object to print
 */
void printDateTime(DateTime dt);

/**
 * Get current time from RTC
 * @return DateTime object with current time
 */
DateTime getCurrentTime();

#endif // RTC_H