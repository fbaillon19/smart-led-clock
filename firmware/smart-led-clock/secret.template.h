/**
 * Smart LED Clock - Secrets Template
 * 
 * Copy this file to 'secret.h' and fill in your actual values.
 * The secret.h file will be ignored by git to keep your credentials safe.
 * 
 * Instructions:
 * 1. Copy this file: cp secret.template.h secret.h
 * 2. Edit secret.h with your actual WiFi credentials
 * 3. Never commit secret.h to version control
 * 
 * Author: F. Baillon
 * Version: 1.0
 * License: MIT
 */

#ifndef SECRET_H
#define SECRET_H

// ==========================================
// WIFI CONFIGURATION
// ==========================================

// Your WiFi network credentials
const char* ssid = "YOUR_WIFI_NETWORK_NAME";
const char* pass = "YOUR_WIFI_PASSWORD";

// ==========================================
// WEB INTERFACE CREDENTIALS (Optional)
// ==========================================

// Uncomment and set if you want to protect the web interface with login
// const char* web_username = "admin";
// const char* web_password = "your_secure_password";

// ==========================================
// API KEYS (Future use)
// ==========================================

// Uncomment if you plan to use weather API services in future updates
// const char* weather_api_key = "your_openweather_api_key";
// const char* location_city = "Paris";
// const char* location_country = "FR";

// ==========================================
// NTP SERVER CONFIGURATION (Optional)
// ==========================================

// You can specify custom NTP servers if needed
// Default: pool.ntp.org is used, which works for most users worldwide
// const char* ntp_server_primary = "pool.ntp.org";
// const char* ntp_server_secondary = "time.nist.gov";

// ==========================================
// MQTT CONFIGURATION (Future use)
// ==========================================

// Uncomment if you plan to send sensor data to MQTT broker for home automation
// const char* mqtt_server = "your_mqtt_broker.local";
// const int mqtt_port = 1883;
// const char* mqtt_user = "your_mqtt_username";
// const char* mqtt_password = "your_mqtt_password";
// const char* mqtt_client_id = "smart-led-clock";

// ==========================================
// DATA LOGGING (Future use)
// ==========================================

// Uncomment if you plan to log data to external services
// const char* logging_endpoint = "https://your-server.com/api/data";
// const char* api_token = "your_api_token";

#endif // SECRET_H