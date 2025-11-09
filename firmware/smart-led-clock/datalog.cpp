/**
 * @file datalog.cpp
 * @brief Data logging implementation
 * 
 * @author F. Baillon
 * @version 1.0.0
 * @date January 2025
 * @license MIT License
 * 
 * Copyright (c) 2025 F. Baillon
 */

#include "datalog.h"


// External flag to prevent I2C conflicts during MQTT
extern bool mqttBusy;

// ==========================================
// GLOBAL VARIABLES
// ==========================================

// Circular buffer for data storage
DataPoint dataBuffer[MAX_DATA_POINTS];
uint16_t bufferWriteIndex = 0;
uint16_t bufferCount = 0;

// Logging statistics
DataLogStats logStats = {0, 0, 0, 0, 0, false};

// MQTT client
extern WiFiClient mqttWifiClient;
PubSubClient mqttClient;

// Timing variables
unsigned long lastLogTime = 0;
unsigned long lastMQTTAttempt = 0;

// ==========================================
// FUNCTION IMPLEMENTATIONS
// ==========================================
void initDataLog(WiFiClient& wifiClient) {
  DEBUG_PRINTLN("Initializing data logging system...");
  
  DEBUG_PRINTLN("Using global WiFi client for MQTT");
  
  // Configure MQTT client avec la variable globale
  mqttClient.setClient(mqttWifiClient);
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

  mqttClient.setBufferSize(512);
  DEBUG_PRINTLN("MQTT buffer size set to 512 bytes");
  
  // Réduire les timeouts
  mqttClient.setSocketTimeout(2);
  mqttClient.setKeepAlive(15);
  
  DEBUG_PRINT("MQTT server configured: ");
  DEBUG_PRINT(MQTT_SERVER);
  DEBUG_PRINT(":");
  DEBUG_PRINTLN(MQTT_PORT);
  
  // Initialize buffer
  bufferWriteIndex = 0;
  bufferCount = 0;
  
  // Reset statistics
  logStats.bufferCount = 0;
  logStats.totalLogged = 0;
  logStats.totalSent = 0;
  logStats.lastLogTime = 0;
  logStats.lastSendTime = 0;
  logStats.mqttConnected = false;
  
  DEBUG_PRINT("Data buffer initialized: ");
  DEBUG_PRINT(MAX_DATA_POINTS);
  DEBUG_PRINT(" points (");
  DEBUG_PRINT(sizeof(dataBuffer) / 1024);
  DEBUG_PRINTLN(" KB)");
  
  if (wifiConnected) {
    DEBUG_PRINTLN("MQTT will connect in background...");
    lastMQTTAttempt = 0;  // Sera à 0, donc première tentative dans 30s
  } else {
    DEBUG_PRINTLN("WiFi not connected, MQTT disabled");
  }
  
  DEBUG_PRINTLN("Data logging initialization complete");
}

void handleDataLog() {
  unsigned long currentMillis = millis();
  
  // ===== DEBUG =====
  static unsigned long lastDebugLog = 0;
  if (currentMillis - lastDebugLog >= 10000) {
    DEBUG_PRINT("handleDataLog: wifiConnected=");
    DEBUG_PRINT(wifiConnected);
    DEBUG_PRINT(" | mqttConnected=");
    DEBUG_PRINT(mqttClient.connected());
    DEBUG_PRINT(" | lastAttempt=");
    DEBUG_PRINT(lastMQTTAttempt);
    DEBUG_PRINT(" | elapsed=");
    DEBUG_PRINTLN(currentMillis - lastMQTTAttempt);
    lastDebugLog = currentMillis;
  }
  // =================
  
  // Handle MQTT connection
  if (wifiConnected) {
    // Reconnect MQTT if disconnected
    if (!mqttClient.connected()) {
      logStats.mqttConnected = false;
      
      // Try to reconnect every 30 seconds
      if (currentMillis - lastMQTTAttempt >= 30000) {
        lastMQTTAttempt = currentMillis;
        
        DEBUG_PRINT("Connecting to MQTT broker ");
        DEBUG_PRINT(MQTT_SERVER);
        DEBUG_PRINT(":");
        DEBUG_PRINT(MQTT_PORT);
        DEBUG_PRINT("...");
        
        // Set flag to prevent I2C access during connection
        mqttBusy = true;
        
        unsigned long startTime = millis();
        
        // Tenter la connexion
        bool connected = mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);
        
        // Clear flag
        mqttBusy = false;
        
        unsigned long elapsed = millis() - startTime;
        
        if (connected) {
          DEBUG_PRINT(" (");
          DEBUG_PRINT(elapsed);
          DEBUG_PRINTLN("ms) ✓ MQTT connected!");
          
          logStats.mqttConnected = true;
          
          // Publier le statut
          if (!mqttClient.publish(MQTT_TOPIC_STATUS, "online")) {
            DEBUG_PRINTLN("Warning: Failed to publish status");
          }
          
          // Send buffered data if any
          if (bufferCount > 0) {
            DEBUG_PRINT("Sending ");
            DEBUG_PRINT(bufferCount);
            DEBUG_PRINTLN(" buffered points...");
            sendBufferToMQTT();
          }
        } else {
          DEBUG_PRINT(" (");
          DEBUG_PRINT(elapsed);
          DEBUG_PRINT("ms) ✗ Failed, rc=");
          DEBUG_PRINTLN(mqttClient.state());
        }
      }
    } else {
      // MQTT connecté : maintenir la connexion
      logStats.mqttConnected = true;
      
      if (mqttClient.connected()) {
        mqttClient.loop();  // Keep MQTT connection alive
      }
    }
  } else {
    logStats.mqttConnected = false;
  }
  
  // Determine logging interval based on WiFi status
  unsigned long logInterval = wifiConnected ? 
                              DATALOG_INTERVAL_WIFI_OK : 
                              DATALOG_INTERVAL_WIFI_DOWN;
  
  // Log data point if interval elapsed
  if (currentMillis - lastLogTime >= logInterval) {
    lastLogTime = currentMillis;
    logDataPoint();
  }
}

bool logDataPoint() {
  // Validate sensor data
  if (!indoorData.valid || !outdoorData.valid || !airQuality.valid) {
    DEBUG_PRINTLN("WARNING: Invalid sensor data, skipping log");
    return false;
  }
  
  DateTime now = getCurrentTime();
  
  // If WiFi OK and MQTT connected, send directly without buffering
  if (wifiConnected && mqttClient.connected()) {
    bool sent = sendMQTTData();
    
    if (sent) {
      logStats.totalLogged++;
      logStats.totalSent++;
      logStats.lastLogTime = millis();
      logStats.lastSendTime = millis();
      
      DEBUG_PRINTLN("Data sent directly via MQTT (no buffering)");
      return true;
    } else {
      // Send failed, fall through to buffering
      DEBUG_PRINTLN("MQTT send failed, storing in buffer");
    }
  }
  
  // WiFi down or MQTT failed: store in buffer
  DataPoint dp;
  dp.timestamp = now.unixtime();
  dp.tempIn = (int16_t)(indoorData.temperature * 10);
  dp.humIn = (uint16_t)(indoorData.humidity * 10);
  dp.tempOut = (int16_t)(outdoorData.temperature * 10);
  dp.humOut = (uint16_t)(outdoorData.humidity * 10);
  dp.aqi = airQuality.estimatedAQI;
  
  // Add to circular buffer
  dataBuffer[bufferWriteIndex] = dp;
  bufferWriteIndex = (bufferWriteIndex + 1) % MAX_DATA_POINTS;
  
  if (bufferCount < MAX_DATA_POINTS) {
    bufferCount++;
  }
  
  // Update statistics
  logStats.bufferCount = bufferCount;
  logStats.totalLogged++;
  logStats.lastLogTime = millis();
  
  DEBUG_PRINT("Data buffered [");
  DEBUG_PRINT(bufferCount);
  DEBUG_PRINT("/");
  DEBUG_PRINT(MAX_DATA_POINTS);
  DEBUG_PRINTLN("]");
  
  return true;
}

bool sendMQTTData() {
  if (!mqttClient.connected()) {
    return false;
  }
  
  DateTime now = getCurrentTime();
  
  // ✅ Use static buffer instead of String (reduced to 384 bytes)
  char json[384];  // Reduced from 512
  
  // Build entire JSON in one snprintf call (no heap allocations)
  int len = snprintf(json, sizeof(json),
    "{"
    "\"timestamp\":\"%04d-%02d-%02dT%02d:%02d:%02dZ\","
    "\"uptime\":%lu,"
    "\"indoor\":{"
      "\"temperature\":%.1f,"
      "\"humidity\":%.1f,"
      "\"dewPoint\":%.1f,"
      "\"humidex\":%d"
    "},"
    "\"outdoor\":{"
      "\"temperature\":%.1f,"
      "\"humidity\":%.1f,"
      "\"dewPoint\":%.1f"
    "},"
    "\"airQuality\":{"
      "\"aqi\":%d,"
      "\"raw\":%d,"
      "\"quality\":\"%s\""
    "},"
    "\"system\":{"
      "\"bufferCount\":%d,"
      "\"bufferMax\":%d"
    "}"
    "}",
    // Timestamp
    now.year(), now.month(), now.day(),
    now.hour(), now.minute(), now.second(),
    // Uptime
    millis() / 1000,
    // Indoor data
    indoorData.temperature,
    indoorData.humidity,
    indoorData.dewPoint,
    indoorData.humidex,
    // Outdoor data
    outdoorData.temperature,
    outdoorData.humidity,
    outdoorData.dewPoint,
    // Air quality
    airQuality.estimatedAQI,
    airQuality.rawADC,
    airQuality.quality,
    // System info
    bufferCount,
    MAX_DATA_POINTS
  );
  
  // Check for buffer overflow
  if (len >= sizeof(json)) {
    DEBUG_PRINTLN("ERROR: JSON buffer overflow!");
    return false;
  }
  
  // Set flag before publish to prevent I2C conflicts
  mqttBusy = true;
  
  // Publish to MQTT
  bool success = mqttClient.publish(MQTT_TOPIC_DATA, json);
  
  // Clear flag after publish
  mqttBusy = false;
  
  if (success) {
    DEBUG_PRINTLN("MQTT data published successfully");
  } else {
    DEBUG_PRINTLN("MQTT publish failed");
  }
  
  return success;
}

bool sendBufferToMQTT() {
  if (!mqttClient.connected() || bufferCount == 0) {
    return false;
  }
  
  DEBUG_PRINT("Sending ");
  DEBUG_PRINT(bufferCount);
  DEBUG_PRINTLN(" buffered points via MQTT...");
  
  uint16_t sent = 0;
  uint16_t readIndex = 0;
  
  // Determine start index for circular buffer
  if (bufferCount == MAX_DATA_POINTS) {
    // Buffer is full, start from write index (oldest data)
    readIndex = bufferWriteIndex;
  } else {
    // Buffer not full, start from beginning
    readIndex = 0;
  }
  
  // Send in chunks
  while (sent < bufferCount) {
    uint16_t remaining = bufferCount - sent;
    uint16_t toSend = (remaining < MQTT_CHUNK_SIZE) ? remaining : MQTT_CHUNK_SIZE;
    
    // ✅ Use static buffer (prevent memory fragmentation)
    char json[384];  // Reduced from 512
    int pos = 0;
    
    // Build JSON header
    pos += snprintf(json + pos, sizeof(json) - pos, "{\"count\":%d,\"data\":[", toSend);
    
    // Add data points
    for (uint16_t i = 0; i < toSend; i++) {
      DataPoint* dp = &dataBuffer[readIndex];
      
      if (i > 0) {
        pos += snprintf(json + pos, sizeof(json) - pos, ",");
      }
      
      pos += snprintf(json + pos, sizeof(json) - pos,
        "{\"ts\":%lu,\"tIn\":%.1f,\"hIn\":%.1f,\"tOut\":%.1f,\"hOut\":%.1f,\"aqi\":%d}",
        dp->timestamp,
        dp->tempIn / 10.0,
        dp->humIn / 10.0,
        dp->tempOut / 10.0,
        dp->humOut / 10.0,
        dp->aqi
      );
      
      readIndex = (readIndex + 1) % MAX_DATA_POINTS;
      
      // Safety: prevent buffer overflow
      if (pos >= sizeof(json) - 20) {
        DEBUG_PRINTLN("WARNING: Buffer chunk too large, sending partial");
        break;
      }
    }
    
    // Close JSON
    pos += snprintf(json + pos, sizeof(json) - pos, "]}");
    
    // Publish chunk
    bool success = mqttClient.publish(MQTT_TOPIC_BUFFER, json);
    
    if (success) {
      sent += toSend;
      logStats.totalSent += toSend;
      
      DEBUG_PRINT("Sent chunk: ");
      DEBUG_PRINT(sent);
      DEBUG_PRINT("/");
      DEBUG_PRINTLN(bufferCount);
      
      // Small delay between chunks
      delay(100);
      mqttClient.loop();  // Keep connection alive
    } else {
      DEBUG_PRINTLN("Buffer send failed");
      return false;
    }
  }
  
  // Clear buffer after successful transmission
  clearBuffer();
  
  DEBUG_PRINTLN("Buffer transmission complete");
  logStats.lastSendTime = millis();
  
  return true;
}

DataLogStats getLogStats() {
  logStats.bufferCount = bufferCount;
  logStats.mqttConnected = mqttClient.connected();
  return logStats;
}

const char* getBufferJSON(uint16_t count) {
  // ✅ Static buffer (reused, no heap allocation) - reduced size
  static char json[768];  // Reduced from 1024 (still larger for web response)
  int pos = 0;
  
  // Limit count for memory safety
  if (count > bufferCount) count = bufferCount;
  if (count > 20) count = 20;  // Reduced from 100 for safety
  
  // JSON header
  pos += snprintf(json + pos, sizeof(json) - pos,
    "{\"count\":%d,\"bufferTotal\":%d,\"data\":[",
    count, bufferCount
  );
  
  if (count == 0) {
    pos += snprintf(json + pos, sizeof(json) - pos, "]}");
    return json;
  }
  
  // Determine start index (get most recent data)
  uint16_t startIndex;
  if (bufferCount < MAX_DATA_POINTS) {
    startIndex = (bufferCount >= count) ? (bufferCount - count) : 0;
  } else {
    startIndex = (bufferWriteIndex + MAX_DATA_POINTS - count) % MAX_DATA_POINTS;
  }
  
  // Build JSON array
  for (uint16_t i = 0; i < count; i++) {
    uint16_t idx = (startIndex + i) % MAX_DATA_POINTS;
    DataPoint* dp = &dataBuffer[idx];
    
    if (i > 0) {
      pos += snprintf(json + pos, sizeof(json) - pos, ",");
    }
    
    pos += snprintf(json + pos, sizeof(json) - pos,
      "{\"ts\":%lu,\"tIn\":%.1f,\"hIn\":%.1f,\"tOut\":%.1f,\"hOut\":%.1f,\"aqi\":%d}",
      dp->timestamp,
      dp->tempIn / 10.0,
      dp->humIn / 10.0,
      dp->tempOut / 10.0,
      dp->humOut / 10.0,
      dp->aqi
    );
    
    // Safety check
    if (pos >= sizeof(json) - 50) {
      DEBUG_PRINTLN("WARNING: JSON buffer near limit");
      break;
    }
  }
  
  // Close JSON
  pos += snprintf(json + pos, sizeof(json) - pos, "]}");
  
  return json;
}

void clearBuffer() {
  bufferWriteIndex = 0;
  bufferCount = 0;
  logStats.bufferCount = 0;
  
  DEBUG_PRINTLN("Data buffer cleared");
}
