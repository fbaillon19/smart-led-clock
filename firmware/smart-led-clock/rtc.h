/**
 * @file rtc.h
 * @brief RTC, WiFi, and NTP management module
 * 
 * Handles DS3231 Real-Time Clock operations, WiFi connectivity,
 * and automatic Network Time Protocol (NTP) synchronization.
 * 
 * Features:
 * - DS3231 RTC initialization and time reading
 * - WiFi connection management
 * - Daily NTP synchronization (configurable time)
 * - Automatic fallback to RTC if NTP fails
 * - DateTime formatting utilities
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
extern RTC_DS3231 rtc;           ///< DS3231 Real-Time Clock object
extern WiFiUDP udp;              ///< UDP client for NTP communication
extern NTPClient timeClient;     ///< NTP client (pool.ntp.org)

// ==========================================
// FUNCTION DECLARATIONS
// ==========================================

/**
 * @brief Initialize the DS3231 RTC module
 * 
 * Performs RTC initialization and checks for power loss.
 * If the RTC lost power (battery died or was removed), a warning
 * is printed to Serial. Time should be synced via NTP after this.
 * 
 * The DS3231 uses I2C communication (SDA/SCL pins).
 * 
 * @return true if RTC initialized successfully, false if not found
 * 
 * @note Call Wire.begin() before this function
 * @see syncTimeWithNTP()
 */
bool initRTC();

/**
 * @brief Connect to WiFi network
 * 
 * Attempts to connect to WiFi using credentials from secrets.h.
 * Tries up to 20 times (10 seconds total) with 500ms delays.
 * 
 * WiFi credentials must be defined in secrets.h:
 * - const char* ssid = "YourSSID";
 * - const char* pass = "YourPassword";
 * 
 * @return true if connected successfully, false if connection failed
 * 
 * @note Arduino Uno R4 WiFi only supports 2.4GHz networks
 * @note Connection status and IP address printed to Serial
 * @see secrets.h, secrets.template.h
 */
bool connectWiFi();

/**
 * @brief Synchronize RTC time with NTP server
 * 
 * Connects to NTP server (pool.ntp.org) and updates the RTC
 * with the current UTC time plus timezone offset from config.h.
 * 
 * Attempts up to 10 times (5 seconds) before giving up.
 * On success, updates the DS3231 RTC and prints new time to Serial.
 * 
 * NTP configuration (config.h):
 * - TIME_ZONE_OFFSET: Hours offset from UTC (e.g., 2 for UTC+2)
 * - NTP_SYNC_HOUR: Hour for daily automatic sync (default: 1 AM)
 * - NTP_SYNC_MINUTE: Minute for daily sync (default: 01)
 * 
 * @return true if sync successful, false if sync failed
 * 
 * @note Requires active WiFi connection
 * @note Time accuracy: typically ±50ms
 * @see connectWiFi(), TIME_ZONE_OFFSET
 */
bool syncTimeWithNTP();

/**
 * @brief Print DateTime object to Serial in formatted way
 * 
 * Outputs date and time in format: YYYY/MM/DD HH:MM:SS
 * Example: 2025/01/15 14:32:45
 * 
 * Uses sprintf for efficient formatting. Output is sent to Serial
 * without newline (use Serial.println() after if needed).
 * 
 * @param dt DateTime object to print (from RTC or NTP)
 * 
 * @note Serial must be initialized before calling (Serial.begin)
 * @see getCurrentTime()
 */
void printDateTime(DateTime dt);

/**
 * @brief Get current time from RTC
 * 
 * Reads and returns current date/time from DS3231 RTC.
 * This is a simple wrapper around rtc.now() for consistency.
 * 
 * The returned DateTime object contains:
 * - year(), month(), day()
 * - hour(), minute(), second()
 * - dayOfTheWeek() (0=Sunday, 6=Saturday)
 * 
 * @return DateTime object with current time
 * 
 * @note If RTC is not initialized, returns invalid DateTime
 * @see initRTC(), printDateTime()
 */
DateTime getCurrentTime();

#endif // RTC_H