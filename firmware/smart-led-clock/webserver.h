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

#include <WiFiS3.h>
#include "config.h"

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
 * @return JSON string with sensor data
 */
String getSensorDataJSON();

/**
 * @brief Get configuration as JSON string
 * @return JSON string with current configuration
 */
String getConfigJSON();

/**
 * @brief Get logging statistics as JSON string
 * @return JSON string with logging stats
 */
String getLogStatsJSON();

/**
 * @brief Parse and save configuration from POST data
 * @param postData POST data string
 * @return true if config saved successfully
 */
bool parseAndSaveConfig(String postData);

/**
 * @brief Send page content in chunks (internal helper)
 */
void sendPageInChunks(WiFiClient& client, const char* content);

#endif // WEBSERVER_H