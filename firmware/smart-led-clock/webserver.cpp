/**
 * @file webserver.cpp
 * @brief Web server implementation - OPTIMIZED (No String leaks)
 * 
 * @author F. Baillon
 * @version 1.2.0
 * @date January 2025
 * @license MIT License
 */

#include "webserver.h"

// ==========================================
// GLOBAL WEB SERVER
// ==========================================
WiFiServer webServer(80);

// ==========================================
// HELPER FUNCTIONS
// ==========================================

/**
 * @brief Extract integer value from buffer (replaces String parsing)
 */
static int extractInt(const char* data, const char* key) {
    char searchStr[64];
    snprintf(searchStr, sizeof(searchStr), "\"%s\":", key);
    
    const char* pos = strstr(data, searchStr);
    if (pos == NULL) return -999;
    
    pos += strlen(searchStr);
    while (*pos == ' ') pos++;  // Skip spaces
    
    return atoi(pos);
}

/**
 * @brief Extract RGB values from nested JSON
 */
static bool extractRGB(const char* data, const char* colorKey, int& r, int& g, int& b) {
    char searchStr[32];
    snprintf(searchStr, sizeof(searchStr), "\"%s\":{", colorKey);
    
    const char* start = strstr(data, searchStr);
    if (start == NULL) return false;
    
    const char* end = strchr(start, '}');
    if (end == NULL) return false;
    
    // Extract substring
    int len = end - start;
    if (len > 200) return false;
    
    char colorData[256];
    strncpy(colorData, start, len);
    colorData[len] = '\0';
    
    r = extractInt(colorData, "r");
    g = extractInt(colorData, "g");
    b = extractInt(colorData, "b");
    
    return (r >= 0 && g >= 0 && b >= 0);
}

// ==========================================
// MAIN HANDLER
// ==========================================

void initWebServer() {
    webServer.begin();
    DEBUG_PRINTLN("Web server started on port 80");
}

void handleWebServer() {
    WiFiClient client = webServer.available();
    
    if (!client) {
        return;
    }
    
    unsigned long startTime = millis();
    DEBUG_PRINTLN("New web client connected");
    
    // ✅ Reduced buffer sizes (was 1024+512 = 1.5KB, now 768+256 = 1KB)
    static char request[768];   // Reduced from 1024
    static char postData[256];  // Reduced from 512
    int requestLen = 0;
    int postDataLen = 0;
    bool isPost = false;
    int contentLength = 0;
    
    // Read HTTP request headers
    while (client.connected() && client.available() && requestLen < sizeof(request) - 1) {
        char c = client.read();
        request[requestLen++] = c;
        request[requestLen] = '\0';
        
        if (strstr(request, "POST") != NULL) {
            isPost = true;
        }
        
        char* contentLengthPos = strstr(request, "Content-Length: ");
        if (contentLengthPos != NULL) {
            contentLength = atoi(contentLengthPos + 16);
        }
        
        // Headers end with \r\n\r\n
        if (requestLen >= 4 && 
            request[requestLen-4] == '\r' && request[requestLen-3] == '\n' &&
            request[requestLen-2] == '\r' && request[requestLen-1] == '\n') {
            break;
        }
    }
    
    // Read POST data if present
    if (isPost && contentLength > 0 && contentLength < sizeof(postData)) {
        unsigned long postStartTime = millis();
        while (postDataLen < contentLength && (millis() - postStartTime) < 5000) {
            if (client.available()) {
                postData[postDataLen++] = (char)client.read();
                postData[postDataLen] = '\0';
            }
        }
    }
    
    // ========================================
    // ROUTE HANDLING
    // ========================================
    
    if (strstr(request, "GET / ") != NULL || strstr(request, "GET /index") != NULL) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        sendPageInChunks(client, WEBPAGE_HOME);
    }
    else if (strstr(request, "GET /config") != NULL) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        sendPageInChunks(client, WEBPAGE_CONFIG);
    }
    else if (strstr(request, "GET /moon") != NULL) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        sendPageInChunks(client, WEBPAGE_MOON);
    }
    else if (strstr(request, "GET /api/status") != NULL) {
        const char* json = getSensorDataJSON();
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println(json);
    }
    else if (strstr(request, "GET /api/config") != NULL) {
        const char* json = getConfigJSON();
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println(json);
    }
    else if (strstr(request, "GET /api/history") != NULL) {
        uint16_t count = 20;
        char* countPos = strstr(request, "count=");
        if (countPos != NULL) {
            count = atoi(countPos + 6);
            if (count == 0 || count > 20) count = 20;
        }
        
        const char* json = getBufferJSON(count);
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println(json);
    }
    else if (strstr(request, "GET /api/logstats") != NULL) {
        const char* json = getLogStatsJSON();
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println(json);
    }
    else if (strstr(request, "GET /api/moon") != NULL) {
        char action[20] = "";
        char* actionPos = strstr(request, "action=");
        if (actionPos != NULL) {
            actionPos += 7;
            int i = 0;
            while (i < 19 && actionPos[i] != ' ' && actionPos[i] != '&' && actionPos[i] != '\0') {
                action[i] = actionPos[i];
                i++;
            }
            action[i] = '\0';
        }
        
        const char* json = getMoonDataJSON(action);
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println(json);
    }
    else if (isPost && strstr(request, "POST /api/config") != NULL) {
        bool success = parseAndSaveConfig(postData, postDataLen);
        
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        
        if (success) {
            client.println("{\"success\":true,\"message\":\"Configuration saved\"}");
        } else {
            client.println("{\"success\":false,\"message\":\"Save failed\"}");
        }
    }
    else {
        client.println("HTTP/1.1 404 Not Found");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.println("404 Not Found");
    }
    
    delay(10);
    client.stop();
    
    DEBUG_PRINT("Request handled in ");
    DEBUG_PRINT(millis() - startTime);
    DEBUG_PRINTLN("ms");
}

// ==========================================
// JSON GENERATION FUNCTIONS (NO STRING)
// ==========================================

const char* getSensorDataJSON() {
    static char json[384];  // Reduced from 512
    
    DateTime now = getCurrentTime();
    
    snprintf(json, sizeof(json),
        "{"
        "\"indoor\":{"
          "\"temp\":%.1f,"
          "\"humidity\":%.1f,"
          "\"valid\":%s"
        "},"
        "\"outdoor\":{"
          "\"temp\":%.1f,"
          "\"humidity\":%.1f,"
          "\"valid\":%s"
        "},"
        "\"airQuality\":{"
          "\"aqi\":%d,"
          "\"quality\":\"%s\""
        "},"
        "\"time\":\"%02d:%02d:%02d\""
        "}",
        indoorData.temperature,
        indoorData.humidity,
        indoorData.valid ? "true" : "false",
        outdoorData.temperature,
        outdoorData.humidity,
        outdoorData.valid ? "true" : "false",
        airQuality.estimatedAQI,
        airQuality.quality,
        now.hour(), now.minute(), now.second()
    );
    
    return json;
}

const char* getConfigJSON() {
    static char json[384];  // Reduced from 512
    
    ClockConfig config;
    getCurrentConfig(&config);
    
    snprintf(json, sizeof(json),
        "{"
        "\"timezoneOffset\":%d,"
        "\"ntpSyncHour\":%d,"
        "\"ntpSyncMinute\":%d,"
        "\"led\":{"
          "\"hour\":{\"r\":%d,\"g\":%d,\"b\":%d},"
          "\"minute\":{\"r\":%d,\"g\":%d,\"b\":%d},"
          "\"second\":{\"r\":%d,\"g\":%d,\"b\":%d},"
          "\"brightness\":%d"
        "},"
        "\"lcdTimeout\":%lu"
        "}",
        config.timezoneOffset,
        config.ntpSyncHour,
        config.ntpSyncMinute,
        config.colorHourR, config.colorHourG, config.colorHourB,
        config.colorMinuteR, config.colorMinuteG, config.colorMinuteB,
        config.colorSecondR, config.colorSecondG, config.colorSecondB,
        config.ledBrightness,
        config.lcdTimeout
    );
    
    return json;
}

const char* getLogStatsJSON() {
    static char json[256];
    
    DataLogStats stats = getLogStats();
    
    snprintf(json, sizeof(json),
        "{"
        "\"bufferCount\":%d,"
        "\"bufferMax\":%d,"
        "\"bufferUsage\":%d,"
        "\"totalLogged\":%d,"
        "\"totalSent\":%d,"
        "\"mqttConnected\":%s,"
        "\"lastLogTime\":%lu,"
        "\"lastSendTime\":%lu"
        "}",
        stats.bufferCount,
        MAX_DATA_POINTS,
        (stats.bufferCount * 100) / MAX_DATA_POINTS,
        stats.totalLogged,
        stats.totalSent,
        stats.mqttConnected ? "true" : "false",
        stats.lastLogTime,
        stats.lastSendTime
    );
    
    return json;
}

const char* getMoonDataJSON(const char* action) {
    static char json[384];  // Reduced from 512
    
    if (strcmp(action, "status") == 0) {
        MoonPhaseData& data = getMoonData();
        
        int pos = snprintf(json, sizeof(json),
            "{"
            "\"phase\":%d,"
            "\"phaseName\":\"%s\","
            "\"exactPhase\":%.3f,"
            "\"illumination\":%.1f,"
            "\"lunarAge\":%.2f,"
            "\"currentSteps\":%d,"
            "\"calibrated\":%s",
            data.phase,
            getMoonPhaseName(data.phase),
            data.exactPhase,
            data.illumination,
            data.lunarAge,
            data.currentSteps,
            data.isCalibrated ? "true" : "false"
        );
        
        if (data.isCalibrated && data.lastCalib > 0) {
            DateTime now = getCurrentTime();
            float daysSinceCalib = daysSinceLastCalibration(now.unixtime());
            pos += snprintf(json + pos, sizeof(json) - pos,
                ",\"daysSinceCalibration\":%.1f",
                daysSinceCalib
            );
        }
        
        snprintf(json + pos, sizeof(json) - pos, "}");
        
    } else if (strcmp(action, "recalibrate") == 0) {
        DEBUG_PRINTLN("[WEB] Manual moon recalibration requested");
        
        MoonCalibrationResult result = calibrateMoonHome();
        
        snprintf(json, sizeof(json),
            "{"
            "\"success\":%s,"
            "\"peakValue\":%d,"
            "\"peakStep\":%d,"
            "\"finalValue\":%d,"
            "\"difference\":%d,"
            "\"duration\":%lu"
            "}",
            result.success ? "true" : "false",
            result.peakValue,
            result.peakStep,
            result.finalValue,
            result.difference,
            result.duration
        );
        
        if (result.success) {
            DateTime now = getCurrentTime();
            updateMoonPosition(now.unixtime());
        }
        
    } else {
        snprintf(json, sizeof(json), "{\"error\":\"Invalid action\"}");
    }
    
    return json;
}

bool parseAndSaveConfig(const char* postData, size_t length) {
    DEBUG_PRINTLN("Parsing config from POST data...");
    
    ClockConfig config;
    getCurrentConfig(&config);
    
    // Parse NTP settings
    int tz = extractInt(postData, "timezoneOffset");
    if (tz != -999) config.timezoneOffset = tz;
    
    int syncH = extractInt(postData, "ntpSyncHour");
    if (syncH != -999) config.ntpSyncHour = syncH;
    
    int syncM = extractInt(postData, "ntpSyncMinute");
    if (syncM != -999) config.ntpSyncMinute = syncM;
    
    // Parse LED colors
    int r, g, b;
    if (extractRGB(postData, "hour", r, g, b)) {
        if (r >= 0 && r <= 255) config.colorHourR = r;
        if (g >= 0 && g <= 255) config.colorHourG = g;
        if (b >= 0 && b <= 255) config.colorHourB = b;
    }
    
    if (extractRGB(postData, "minute", r, g, b)) {
        if (r >= 0 && r <= 255) config.colorMinuteR = r;
        if (g >= 0 && g <= 255) config.colorMinuteG = g;
        if (b >= 0 && b <= 255) config.colorMinuteB = b;
    }
    
    if (extractRGB(postData, "second", r, g, b)) {
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
    
    // Save to EEPROM
    bool saved = saveConfig(&config);
    
    if (saved) {
        DEBUG_PRINTLN("Config saved successfully");
        applyConfig(&config);
    }
    
    return saved;
}

void sendPageInChunks(WiFiClient& client, const char* content) {
    const size_t CHUNK_SIZE = 512;
    char buffer[CHUNK_SIZE + 1];
    size_t contentLen = strlen_P(content);
    size_t sent = 0;
    
    while (sent < contentLen) {
        size_t toSend = min(CHUNK_SIZE, contentLen - sent);
        memcpy_P(buffer, content + sent, toSend);
        buffer[toSend] = '\0';
        
        client.write((uint8_t*)buffer, toSend);
        sent += toSend;
        
        delay(1);
    }
}
