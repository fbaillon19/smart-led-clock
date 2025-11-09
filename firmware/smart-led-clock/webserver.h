/**
 * @file webserver.h
 * @brief Simple web server for Smart LED Clock
 * 
 * Provides a basic HTTP server to display sensor data and configuration.
 * 
 * Features:
 * - Web interface on port 80
 * - JSON API endpoint for sensor data
 * - Simple HTML page
 * 
 * @author F. Baillon
 * @version 1.1.0
 * @date January 2025
 * @license MIT License
 */

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <Arduino.h>
#include <WiFiS3.h>
#include "config.h"
#include "rtc.h"
#include "storage.h"
#include "leds.h"
#include "webpage.h"
#include "datalog.h"
#include "moon.h"


// ==========================================
// WEB SERVER OBJECT
// ==========================================
extern WiFiServer webServer;

// ==========================================
// FUNCTION DECLARATIONS
// ==========================================

/**
 * @brief Initialize web server
 * 
 * Starts the web server on port 80.
 * Call this after WiFi is connected.
 */
void initWebServer();

/**
 * @brief Handle incoming web requests
 * 
 * Call this in main loop() to process HTTP requests.
 * Non-blocking - returns immediately if no client.
 */
void handleWebServer();

/**
 * @brief Get sensor data as JSON string
 * 
 * Formats current sensor readings into JSON format.
 * 
 * ⚠️ Returns pointer to static buffer - valid until next call
 * 
 * @return Pointer to static JSON buffer
 */
const char* getSensorDataJSON();

/**
 * @brief Get moon phase data as JSON string
 * 
 * Formats moon phase data into JSON format based on action parameter.
 * 
 * ⚠️ Returns pointer to static buffer - valid until next call
 * 
 * @param action Action to perform ("status" or "recalibrate")
 * @return Pointer to static JSON buffer
 */
const char* getMoonDataJSON(const char* action);

/**
 * @brief Get configuration as JSON string
 * 
 * ⚠️ Returns pointer to static buffer - valid until next call
 * 
 * @return Pointer to static JSON buffer
 */
const char* getConfigJSON();

/**
 * @brief Get logging statistics as JSON string
 * 
 * ⚠️ Returns pointer to static buffer - valid until next call
 * 
 * @return Pointer to static JSON buffer
 */
const char* getLogStatsJSON();

/**
 * @brief Parse and save configuration from POST data
 * @param postData POST data buffer (null-terminated)
 * @param length Length of POST data
 * @return true if config saved successfully
 */
bool parseAndSaveConfig(const char* postData, size_t length);

/**
 * @brief Send page content in chunks (internal helper)
 */
void sendPageInChunks(WiFiClient& client, const char* content);

#endif // WEBSERVER_H