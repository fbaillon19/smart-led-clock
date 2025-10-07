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

// ==========================================
// GLOBAL WEB SERVER
// ==========================================
WiFiServer webServer(80);

// ==========================================
// HTML PAGE (stored in PROGMEM to save RAM)
// ==========================================
const char WEBPAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart LED Clock</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 50px auto;
            padding: 20px;
            background: #f0f0f0;
        }
        .container {
            background: white;
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        h1 {
            color: #333;
            text-align: center;
        }
        .sensor-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 20px;
            margin-top: 30px;
        }
        .sensor-card {
            background: #f9f9f9;
            padding: 20px;
            border-radius: 8px;
            border-left: 4px solid #4CAF50;
        }
        .sensor-label {
            font-size: 14px;
            color: #666;
            margin-bottom: 5px;
        }
        .sensor-value {
            font-size: 28px;
            font-weight: bold;
            color: #333;
        }
        .sensor-unit {
            font-size: 18px;
            color: #999;
        }
        .update-time {
            text-align: center;
            color: #999;
            margin-top: 20px;
        }
        .refresh-btn {
            display: block;
            width: 200px;
            margin: 20px auto;
            padding: 12px;
            background: #4CAF50;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            font-size: 16px;
        }
        .refresh-btn:hover {
            background: #45a049;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🕐 Smart LED Clock</h1>
        
        <div class="sensor-grid">
            <div class="sensor-card">
                <div class="sensor-label">Température Intérieure</div>
                <div class="sensor-value" id="tempIn">--</div>
                <div class="sensor-unit">°C</div>
            </div>
            
            <div class="sensor-card">
                <div class="sensor-label">Humidité Intérieure</div>
                <div class="sensor-value" id="humIn">--</div>
                <div class="sensor-unit">%</div>
            </div>
            
            <div class="sensor-card">
                <div class="sensor-label">Température Extérieure</div>
                <div class="sensor-value" id="tempOut">--</div>
                <div class="sensor-unit">°C</div>
            </div>
            
            <div class="sensor-card">
                <div class="sensor-label">Humidité Extérieure</div>
                <div class="sensor-value" id="humOut">--</div>
                <div class="sensor-unit">%</div>
            </div>
            
            <div class="sensor-card">
                <div class="sensor-label">Qualité de l'Air (AQI)</div>
                <div class="sensor-value" id="aqi">--</div>
                <div class="sensor-unit" id="aqiQuality">--</div>
            </div>
            
            <div class="sensor-card">
                <div class="sensor-label">Heure</div>
                <div class="sensor-value" id="time" style="font-size: 20px;">--:--:--</div>
            </div>
        </div>
        
        <button class="refresh-btn" onclick="updateData()">🔄 Rafraîchir</button>
        
        <div class="update-time">
            Dernière mise à jour: <span id="lastUpdate">--</span>
        </div>
    </div>
    
    <script>
        function updateData() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('tempIn').textContent = data.indoor.temp.toFixed(1);
                    document.getElementById('humIn').textContent = Math.round(data.indoor.humidity);
                    document.getElementById('tempOut').textContent = data.outdoor.temp.toFixed(1);
                    document.getElementById('humOut').textContent = Math.round(data.outdoor.humidity);
                    document.getElementById('aqi').textContent = data.airQuality.aqi;
                    document.getElementById('aqiQuality').textContent = data.airQuality.quality;
                    document.getElementById('time').textContent = data.time;
                    document.getElementById('lastUpdate').textContent = new Date().toLocaleTimeString('fr-FR');
                })
                .catch(error => {
                    console.error('Error:', error);
                    alert('Erreur de connexion');
                });
        }
        
        // Auto-refresh every 5 seconds
        setInterval(updateData, 5000);
        
        // Initial load
        updateData();
    </script>
</body>
</html>
)rawliteral";

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
    
    DEBUG_PRINTLN("New web client connected");
    
    String request = "";
    
    // Read the HTTP request
    while (client.connected() && client.available()) {
        char c = client.read();
        request += c;
        
        // Request ends with blank line
        if (request.endsWith("\r\n\r\n")) {
            break;
        }
        
        // Prevent buffer overflow
        if (request.length() > 500) {
            break;
        }
    }
    
    DEBUG_PRINT("Request: ");
    DEBUG_PRINTLN(request.substring(0, 50));  // Print first 50 chars
    
    // Route the request
    if (request.indexOf("GET / ") >= 0) {
        // Serve main HTML page
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        client.println(WEBPAGE);
        
    } else if (request.indexOf("GET /api/status") >= 0) {
        // Serve JSON data
        String json = getSensorDataJSON();
        
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println(json);
        
    } else {
        // 404 Not Found
        client.println("HTTP/1.1 404 Not Found");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.println("404 - Page not found");
    }
    
    // Give client time to receive data
    delay(10);
    client.stop();
    
    DEBUG_PRINTLN("Client disconnected");
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