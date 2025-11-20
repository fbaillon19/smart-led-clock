/**
 * @file datalog.h
 * @brief Data logging module with adaptive WiFi strategy
 * 
 * Manages sensor data logging with intelligent buffering:
 * - WiFi OK: Send immediately every 2 minutes (no local storage)
 * - WiFi DOWN: Store in RAM buffer every 5 minutes (48h capacity)
 * - WiFi RESTORED: Send buffered data to MQTT
 * 
 * Memory usage: ~8 KB RAM for 576 data points (48h at 5min interval)
 * 
 * @author F. Baillon
 * @version 1.1.0
 * @date November 2025
 * @license MIT License
 * 
 * Copyright (c) 2025 F. Baillon
 */

#ifndef DATALOG_H
#define DATALOG_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiS3.h>
#include "config.h"
#include "rtc.h"


// ==========================================
// DATA LOGGING CONFIGURATION
// ==========================================

/**
 * Logging intervals
 */
#define DATALOG_INTERVAL_WIFI_OK    120000  ///< 2 minutes when WiFi connected
#define DATALOG_INTERVAL_WIFI_DOWN  300000  ///< 5 minutes when WiFi down
#define MQTT_RETRY_INTERVAL         600000  ///< Retry MQTT connection every 10 min

/**
 * Buffer configuration
 */
#define MAX_DATA_POINTS             200     ///< 16h40 at 5min intervals (8 KB RAM)
#define MQTT_CHUNK_SIZE             10      ///< Send 10 points per MQTT message

/**
 * MQTT configuration
 */
#define MQTT_TOPIC_DATA             "home/clock/sensors"
#define MQTT_TOPIC_BUFFER           "home/clock/buffer"
#define MQTT_TOPIC_STATUS           "home/clock/status"

// ==========================================
// DATA STRUCTURES
// ==========================================

/**
 * @struct DataPoint
 * @brief Compact sensor data point for buffer storage
 * 
 * Size: 14 bytes per point
 * Total buffer size: 576 points * 14 bytes = 8064 bytes (~8 KB)
 */
struct DataPoint {
  uint32_t timestamp;     ///< Unix timestamp (4 bytes)
  int16_t tempIn;         ///< Indoor temp * 10 (2 bytes, e.g., 21.5°C = 215)
  uint16_t humIn;         ///< Indoor humidity * 10 (2 bytes)
  int16_t tempOut;        ///< Outdoor temp * 10 (2 bytes)
  uint16_t humOut;        ///< Outdoor humidity * 10 (2 bytes)
  uint16_t aqi;           ///< Air Quality Index (2 bytes)
};

/**
 * @struct DataLogStats
 * @brief Statistics about data logging
 */
struct DataLogStats {
  uint16_t bufferCount;           ///< Current number of points in buffer
  uint16_t totalLogged;           ///< Total points logged since boot
  uint16_t totalSent;             ///< Total points sent via MQTT
  unsigned long lastLogTime;      ///< Timestamp of last log
  unsigned long lastSendTime;     ///< Timestamp of last MQTT send
  bool mqttConnected;             ///< MQTT connection status
};

// ==========================================
// GLOBAL VARIABLES
// ==========================================

extern WiFiClient mqttWifiClient;
extern DataPoint dataBuffer[MAX_DATA_POINTS];
extern uint16_t bufferWriteIndex;
extern uint16_t bufferCount;
extern DataLogStats logStats;
extern PubSubClient mqttClient;

// ==========================================
// FUNCTION DECLARATIONS
// ==========================================

/**
 * @brief Initialize data logging system
 * 
 * Sets up MQTT client and initializes buffer.
 * Call once in setup() after WiFi is initialized.
 * 
 * @param wifiClient Reference to WiFiClient for MQTT
 */
void initDataLog(WiFiClient& wifiClient);

/**
 * @brief Main data logging loop handler
 * 
 * Call this in main loop(). Handles:
 * - Adaptive logging (2min WiFi OK / 5min WiFi DOWN)
 * - MQTT connection management
 * - Automatic buffer transmission when WiFi restored
 * 
 * Non-blocking, returns immediately if nothing to do.
 */
void handleDataLog();

/**
 * @brief Log current sensor data
 * 
 * Behavior depends on WiFi status:
 * - WiFi OK: Send immediately via MQTT (no buffer storage)
 * - WiFi DOWN: Store in circular buffer
 * 
 * @return true if logged successfully
 */
bool logDataPoint();

/**
 * @brief Send current sensor data via MQTT
 * 
 * Sends real-time data to MQTT broker.
 * 
 * @return true if sent successfully
 */
bool sendMQTTData();

/**
 * @brief Send buffered data to MQTT
 * 
 * Sends all buffered data points in chunks.
 * Called automatically when WiFi is restored.
 * 
 * @return true if all data sent successfully
 */
bool sendBufferToMQTT();

/**
 * @brief Get data logging statistics
 * 
 * @return Current logging statistics
 */
DataLogStats getLogStats();

/**
 * @brief Get buffer data as JSON
 * 
 * Returns last N data points from buffer as JSON array.
 * Used for web API endpoint.
 * 
 * ⚠️ WARNING: Returns pointer to static buffer - copy immediately if needed!
 * The buffer will be overwritten on next call.
 * 
 * @param count Number of points to retrieve (max 20 for memory safety)
 * @return Pointer to static JSON buffer (valid until next call)
 */
const char* getBufferJSON(uint16_t count);

/**
 * @brief Clear all buffered data
 * 
 * Resets buffer to empty state.
 * Use with caution - data will be lost!
 */
void clearBuffer();

#endif // DATALOG_H
