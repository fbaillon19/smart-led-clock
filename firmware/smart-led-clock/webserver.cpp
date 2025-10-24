/**
 * @file webserver.cpp
 * @brief Web server implementation
 * 
 * @author F. Baillon
 * @version 1.1.0
 * @date January 2025
 * @license MIT License
 */

#include "webserver.h"
#include "rtc.h"
#include <Arduino.h>
#include "storage.h"
#include "leds.h"
#include "webpage.h"
#include "datalog.h"

// ==========================================
// GLOBAL WEB SERVER
// ==========================================
WiFiServer webServer(80);

// ==========================================
// FUNCTION IMPLEMENTATIONS
// ==========================================

void initWebServer() {
    webServer.begin();
    DEBUG_PRINTLN("Web server started on port 80");
}

void handleWebServer() {
    WiFiClient client = webServer.available();
    
    if (!client) {
        return;  // No client, return immediately
    }
    
    unsigned long startTime = millis();  // ⏱️ Début

    DEBUG_PRINTLN("New web client connected");
    
    String request = "";
    String postData = "";
    bool isPost = false;
    int contentLength = 0;
    
    // Read the HTTP request headers
    while (client.connected() && client.available()) {
        char c = client.read();
        request += c;
        
        // Check if it's a POST request
        if (request.indexOf("POST") >= 0) {
            isPost = true;
        }
        
        // Extract Content-Length for POST
        if (request.indexOf("Content-Length: ") >= 0) {
            int startPos = request.indexOf("Content-Length: ") + 16;
            int endPos = request.indexOf("\r\n", startPos);
            String lengthStr = request.substring(startPos, endPos);
            contentLength = lengthStr.toInt();
        }
        
        // Headers end with blank line
        if (request.endsWith("\r\n\r\n")) {
            break;
        }
        
        // Prevent buffer overflow
        if (request.length() > 1000) {
            break;
        }
    }
    
    // Read POST data if present
    if (isPost && contentLength > 0) {
        unsigned long startTime = millis();
        while (postData.length() < contentLength && (millis() - startTime) < 5000) {
            if (client.available()) {
                postData += (char)client.read();
            }
        }
        DEBUG_PRINT("POST data: ");
        DEBUG_PRINTLN(postData);
    }
    
    DEBUG_PRINT("Request: ");
    DEBUG_PRINTLN(request.substring(0, 50));  // Print first 50 chars
    
    // ========================================
    // ROUTE HANDLING
    // ========================================
    
    // Route: Main page
    if (request.indexOf("GET / ") >= 0 || request.indexOf("GET /index") >= 0) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        
        sendPageInChunks(client, WEBPAGE_HOME);
    }
    
    // Route: Configuration page
    else if (request.indexOf("GET /config") >= 0) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        
        sendPageInChunks(client, WEBPAGE_CONFIG);
    }
    
    // Route: API Status (GET)
    else if (request.indexOf("GET /api/status") >= 0) {
        String json = getSensorDataJSON();
        
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println(json);
    }
    
    // Route: API Config (GET)
    else if (request.indexOf("GET /api/config") >= 0) {
        String json = getConfigJSON();
        
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println(json);
    }
    
    // Route: API History (GET) - New in Phase 3.3
    else if (request.indexOf("GET /api/history") >= 0) {
        // Extract count parameter if present (e.g., /api/history?count=50)
        uint16_t count = 100;  // Default
        int countPos = request.indexOf("count=");
        if (countPos >= 0) {
            int endPos = request.indexOf("&", countPos);
            if (endPos < 0) endPos = request.indexOf(" ", countPos);
            String countStr = request.substring(countPos + 6, endPos);
            count = countStr.toInt();
            if (count == 0) count = 100;
        }
        
        String json = getBufferJSON(count);
        
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println(json);
    }
    
    // Route: API Logging Stats (GET) - New in Phase 3.3
    else if (request.indexOf("GET /api/logstats") >= 0) {
        String json = getLogStatsJSON();
        
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println(json);
    }
    
    // Route: API Config (POST)
    else if (request.indexOf("POST /api/config") >= 0) {
        bool success = parseAndSaveConfig(postData);
        
        String json = "{\"success\":";
        json += success ? "true" : "false";
        json += ",\"message\":\"";
        json += success ? "Config saved" : "Failed to save config";
        json += "\"}";
        
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println(json);
    }
    
    // Route: 404 Not Found
    else {
        client.println("HTTP/1.1 404 Not Found");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.println("404 - Page not found");
    }
    
    // Give client time to receive data
    delay(1);
    client.stop();
    
    DEBUG_PRINTLN("Client disconnected");

    unsigned long duration = millis() - startTime;  // ⏱️ Fin
    DEBUG_PRINT("Request handled in ");
    DEBUG_PRINT(duration);
    DEBUG_PRINTLN(" ms");
}

String getSensorDataJSON() {
    DateTime now = getCurrentTime();
    
    String json = "{";
    
    // Indoor sensor
    json += "\"indoor\":{";
    json += "\"temp\":" + String(indoorData.temperature, 1) + ",";
    json += "\"humidity\":" + String(indoorData.humidity, 1) + ",";
    json += "\"valid\":" + String(indoorData.valid ? "true" : "false");
    json += "},";
    
    // Outdoor sensor
    json += "\"outdoor\":{";
    json += "\"temp\":" + String(outdoorData.temperature, 1) + ",";
    json += "\"humidity\":" + String(outdoorData.humidity, 1) + ",";
    json += "\"valid\":" + String(outdoorData.valid ? "true" : "false");
    json += "},";
    
    // Air quality
    json += "\"airQuality\":{";
    json += "\"aqi\":" + String(airQuality.estimatedAQI) + ",";
    json += "\"quality\":\"" + String(airQuality.quality) + "\"";
    json += "},";
    
    // Time
    char timeStr[10];
    sprintf(timeStr, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
    json += "\"time\":\"" + String(timeStr) + "\"";
    
    json += "}";
    
    return json;
}

String getConfigJSON() {
    ClockConfig config;
    getCurrentConfig(&config);
    
    String json = "{";
    
    // NTP settings
    json += "\"timezoneOffset\":" + String(config.timezoneOffset) + ",";
    json += "\"ntpSyncHour\":" + String(config.ntpSyncHour) + ",";
    json += "\"ntpSyncMinute\":" + String(config.ntpSyncMinute) + ",";
    
    // LED settings
    json += "\"led\":{";
    json += "\"hour\":{";
    json += "\"r\":" + String(config.colorHourR) + ",";
    json += "\"g\":" + String(config.colorHourG) + ",";
    json += "\"b\":" + String(config.colorHourB);
    json += "},";
    json += "\"minute\":{";
    json += "\"r\":" + String(config.colorMinuteR) + ",";
    json += "\"g\":" + String(config.colorMinuteG) + ",";
    json += "\"b\":" + String(config.colorMinuteB);
    json += "},";
    json += "\"second\":{";
    json += "\"r\":" + String(config.colorSecondR) + ",";
    json += "\"g\":" + String(config.colorSecondG) + ",";
    json += "\"b\":" + String(config.colorSecondB);
    json += "},";
    json += "\"brightness\":" + String(config.ledBrightness);
    json += "},";
    
    // LCD settings
    json += "\"lcdTimeout\":" + String(config.lcdTimeout) + ",";
    
    // Language
    json += "\"language\":" + String(config.language) + ",";
    
    // Debug mode
    json += "\"debugMode\":" + String(config.debugMode);
    
    json += "}";
    
    return json;
}

bool parseAndSaveConfig(String postData) {
    DEBUG_PRINTLN("Parsing config from POST data...");
    
    // Simple JSON parsing (manual for embedded systems)
    ClockConfig config;
    getCurrentConfig(&config);  // Start with current config
    
    // Helper function to extract integer value
    auto extractInt = [](String data, String key) -> int {
        int startPos = data.indexOf("\"" + key + "\":");
        if (startPos < 0) return -999;  // Key not found
        startPos = data.indexOf(":", startPos) + 1;
        int endPos = data.indexOf(",", startPos);
        if (endPos < 0) endPos = data.indexOf("}", startPos);
        String value = data.substring(startPos, endPos);
        value.trim();
        return value.toInt();
    };
    
    // Parse NTP settings
    int tz = extractInt(postData, "timezoneOffset");
    if (tz != -999) config.timezoneOffset = tz;
    
    int syncH = extractInt(postData, "ntpSyncHour");
    if (syncH != -999) config.ntpSyncHour = syncH;
    
    int syncM = extractInt(postData, "ntpSyncMinute");
    if (syncM != -999) config.ntpSyncMinute = syncM;
    
    // Parse LED colors - Hour
    int hr = extractInt(postData, "\"hour\":{\"r\"");
    if (hr == -999) hr = extractInt(postData, "r");  // Try simplified key
    if (hr != -999 && hr >= 0 && hr <= 255) config.colorHourR = hr;
    
    // This is getting complex - let me use a better approach
    // Extract LED hour colors
    int hourStart = postData.indexOf("\"hour\":{");
    if (hourStart >= 0) {
        int hourEnd = postData.indexOf("}", hourStart);
        String hourData = postData.substring(hourStart, hourEnd);
        
        int r = extractInt(hourData, "r");
        int g = extractInt(hourData, "g");
        int b = extractInt(hourData, "b");
        
        if (r >= 0 && r <= 255) config.colorHourR = r;
        if (g >= 0 && g <= 255) config.colorHourG = g;
        if (b >= 0 && b <= 255) config.colorHourB = b;
    }
    
    // Extract LED minute colors
    int minuteStart = postData.indexOf("\"minute\":{");
    if (minuteStart >= 0) {
        int minuteEnd = postData.indexOf("}", minuteStart);
        String minuteData = postData.substring(minuteStart, minuteEnd);
        
        int r = extractInt(minuteData, "r");
        int g = extractInt(minuteData, "g");
        int b = extractInt(minuteData, "b");
        
        if (r >= 0 && r <= 255) config.colorMinuteR = r;
        if (g >= 0 && g <= 255) config.colorMinuteG = g;
        if (b >= 0 && b <= 255) config.colorMinuteB = b;
    }
    
    // Extract LED second colors
    int secondStart = postData.indexOf("\"second\":{");
    if (secondStart >= 0) {
        int secondEnd = postData.indexOf("}", secondStart);
        String secondData = postData.substring(secondStart, secondEnd);
        
        int r = extractInt(secondData, "r");
        int g = extractInt(secondData, "g");
        int b = extractInt(secondData, "b");
        
        if (r >= 0 && r <= 255) config.colorSecondR = r;
        if (g >= 0 && g <= 255) config.colorSecondG = g;
        if (b >= 0 && b <= 255) config.colorSecondB = b;
    }
    
    // Parse brightness
    int bright = extractInt(postData, "brightness");
    if (bright >= 0 && bright <= 255) config.ledBrightness = bright;
    
    // Parse LCD timeout
    int lcdTime = extractInt(postData, "lcdTimeout");
    if (lcdTime >= 5000) config.lcdTimeout = lcdTime;
    
    // Parse language
    int lang = extractInt(postData, "language");
    if (lang == 0 || lang == 1) config.language = lang;
    
    // Parse debug mode
    int debug = extractInt(postData, "debugMode");
    if (debug == 0 || debug == 1) config.debugMode = debug;
    
    // Save to EEPROM
    bool saved = saveConfig(&config);
    
    if (saved) {
        DEBUG_PRINTLN("Config saved successfully");
        // Apply new config (will need restart for some settings)
        applyConfig(&config);

          // Forcer la mise à jour immédiate de l'affichage LED
        DateTime now = getCurrentTime();
        updateLEDClock(now);

    } else {
        DEBUG_PRINTLN("Config unchanged or save failed");
    }
    
    return true;  // Return true even if unchanged (not an error)
}

/**
 * @brief Send page content in chunks for better performance
 * 
 * Sends PROGMEM content in 512-byte chunks instead of byte-by-byte.
 * This dramatically improves transfer speed.
 * 
 * @param client WiFiClient to send data to
 * @param content PROGMEM string containing page content
 */
void sendPageInChunks(WiFiClient& client, const char* content) {
    const size_t CHUNK_SIZE = 512;  // Optimal chunk size
    char buffer[CHUNK_SIZE + 1];
    size_t contentLen = strlen_P(content);
    size_t sent = 0;
    
    while (sent < contentLen) {
        size_t toSend = min(CHUNK_SIZE, contentLen - sent);
        memcpy_P(buffer, content + sent, toSend);
        buffer[toSend] = '\0';
        
        client.write((uint8_t*)buffer, toSend);
        sent += toSend;
        
        // Small delay to avoid buffer overflow
        delay(1);
    }
}

/**
 * @brief Get logging statistics as JSON
 * 
 * Returns current data logging statistics including buffer status,
 * MQTT connection, and counters.
 * 
 * @return JSON string with logging statistics
 */
String getLogStatsJSON() {
    DataLogStats stats = getLogStats();
    
    String json = "{";
    json += "\"bufferCount\":" + String(stats.bufferCount) + ",";
    json += "\"bufferMax\":" + String(MAX_DATA_POINTS) + ",";
    json += "\"bufferUsage\":" + String((stats.bufferCount * 100) / MAX_DATA_POINTS) + ",";
    json += "\"totalLogged\":" + String(stats.totalLogged) + ",";
    json += "\"totalSent\":" + String(stats.totalSent) + ",";
    json += "\"mqttConnected\":" + String(stats.mqttConnected ? "true" : "false") + ",";
    json += "\"lastLogTime\":" + String(stats.lastLogTime) + ",";
    json += "\"lastSendTime\":" + String(stats.lastSendTime);
    json += "}";
    
    return json;
}