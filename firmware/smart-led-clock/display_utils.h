/**
 * Smart LED Clock - LCD Display Utilities
 * 
 * This file handles LCD display functions including:
 * - Temperature and humidity display modes
 * - User interface management
 * - Display mode switching
 * - Custom characters and formatting
 * 
 * Author: F. Baillon
 * Version: 1.0
 * License: GPL v3.0
 */

#ifndef DISPLAY_UTILS_H
#define DISPLAY_UTILS_H

#include <LiquidCrystal_I2C.h>
#include "config.h"
#include "sensors.h"

// ==========================================
// DISPLAY MODE DEFINITIONS
// ==========================================

enum DisplayMode {
  MODE_TEMP_HUMIDITY = 0,     // Default: Temperature and Humidity
  MODE_FEELS_LIKE = 1,        // Feels-like temp and dew point
  MODE_HUMIDEX = 2,           // Humidex display
  MODE_DATE_TIME = 3,         // Date and time display
  MODE_SYSTEM_INFO = 4        // WiFi, IP, etc.
};

// ==========================================
// GLOBAL VARIABLES
// ==========================================
// LCD object
extern LiquidCrystal_I2C lcd;

// Button objects
extern OneButton button1;
extern OneButton button2;

// Display state
extern DisplayMode currentDisplayMode;
extern bool isTemporaryDisplay;
extern unsigned long temporaryDisplayTimer;
extern bool isPermanentFeelsLike;

// Custom characters
extern byte degreeSymbol[8];

// ==========================================
// INITIALIZATION FUNCTIONS
// ==========================================

/**
 * Initialize LCD display and buttons
 * Sets up I2C communication and button callbacks
 */
void initDisplay();

/**
 * Setup custom characters for LCD
 * Creates degree symbol and other special chars
 */
void setupCustomCharacters();

/**
 * Setup button handlers with callbacks
 */
void setupButtons();

// ==========================================
// MAIN DISPLAY FUNCTIONS
// ==========================================

/**
 * Update LCD display based on current mode and sensor data
 * Called regularly from main loop
 */
void updateLCDDisplay();

/**
 * Display temperature and humidity (default mode)
 * Shows indoor data on lines 0-1, outdoor on lines 2-3
 */
void displayTemperatureHumidity() {
  // Implementation here
}

/**
 * Display feels-like temperature and dew point
 * Only shows outdoor data (more relevant)
 */
void displayFeelsLikeDewPoint() {
  // Implementation here
}

/**
 * Display humidex value with comfort interpretation
 */
void displayHumidex() {
  // Implementation here
}

/**
 * Display current date and time
 */
void displayDateTime() {
  // Implementation here
}

/**
 * Display system information (IP, WiFi status, etc.)
 */
void displaySystemInfo() {
  // Implementation here
}

// ==========================================
// SENSOR DATA DISPLAY
// ==========================================

/**
 * Display temperature data for one sensor
 * @param data Temperature data structure
 * @param startLine LCD line to start display (0-3)
 * @param label Sensor label ("INDOOR" or "OUTDOOR")
 */
void displaySensorData(const TemperatureData& data, int startLine, const char* label) {
  // Implementation here
}

/**
 * Display error message for failed sensor
 * @param startLine LCD line to start display
 * @param sensorName Name of failed sensor
 */
void displaySensorError(int startLine, const char* sensorName) {
  // Implementation here
}

/**
 * Format temperature value with degree symbol
 * @param temp Temperature value
 * @param buffer Output buffer (minimum 8 chars)
 */
void formatTemperature(float temp, char* buffer) {
  // Implementation here
}

/**
 * Format humidity value with percentage
 * @param humidity Humidity value
 * @param buffer Output buffer (minimum 8 chars)
 */
void formatHumidity(float humidity, char* buffer) {
  // Implementation here
}

// ==========================================
// HUMIDEX FUNCTIONS
// ==========================================

/**
 * Get humidex comfort description
 * @param humidexValue Calculated humidex value
 * @return Text description of comfort level
 */
const char* getHumidexDescription(int humidexValue) {
  // Implementation here
  return "Normal";
}

/**
 * Display humidex with interpretation
 * @param humidexValue Humidex value to display
 * @param startLine Starting line on LCD
 */
void displayHumidexWithDescription(int humidexValue, int startLine) {
  // Implementation here
}

// ==========================================
// DISPLAY MODE MANAGEMENT
// ==========================================

/**
 * Switch to temporary display mode
 * @param mode Mode to display temporarily
 * @param duration Duration in milliseconds
 */
void setTemporaryDisplay(DisplayMode mode, unsigned long duration) {
  currentDisplayMode = mode;
  isTemporaryDisplay = true;
  temporaryDisplayTimer = millis() + duration;
}

/**
 * Clear temporary display and return to default
 */
void clearTemporaryDisplay() {
  isTemporaryDisplay = false;
  currentDisplayMode = isPermanentFeelsLike ? MODE_FEELS_LIKE : MODE_TEMP_HUMIDITY;
}

/**
 * Check if temporary display has expired
 * @return true if display should return to default
 */
bool hasTemporaryDisplayExpired() {
  return isTemporaryDisplay && (millis() > temporaryDisplayTimer);
}

/**
 * Toggle permanent feels-like mode
 */
void togglePermanentFeelsLike() {
  isPermanentFeelsLike = !isPermanentFeelsLike;
  if (!isTemporaryDisplay) {
    currentDisplayMode = isPermanentFeelsLike ? MODE_FEELS_LIKE : MODE_TEMP_HUMIDITY;
  }
}

// Note: Button callback functions have been moved to button.h

/**
 * Clear entire LCD display
 */
void clearLCDDisplay() {
  lcd.clear();
}

/**
 * Clear specific LCD line
 * @param line Line number (0-3)
 */
void clearLCDLine(int line) {
  lcd.setCursor(0, line);
  lcd.print("                    "); // 20 spaces
}

/**
 * Center text on LCD line
 * @param text Text to center
 * @param line Line number (0-3)
 */
void centerTextOnLine(const char* text, int line) {
  int textLen = strlen(text);
  int spaces = (LCD_COLUMNS - textLen) / 2;
  
  lcd.setCursor(0, line);
  for (int i = 0; i < spaces; i++) {
    lcd.print(" ");
  }
  lcd.print(text);
}

/**
 * Display startup message
 * @param message Message to display during startup
 */
void displayStartupMessage(const char* message) {
  lcd.clear();
  centerTextOnLine("Smart LED Clock", 0);
  centerTextOnLine("v1.0", 1);
  centerTextOnLine(message, 3);
}

/**
 * Display connection status during WiFi setup
 * @param status Current connection status
 */
void displayConnectionStatus(const char* status) {
  lcd.setCursor(0, 2);
  lcd.print("Status: ");
  lcd.print(status);
}

/**
 * Update display progress bar
 * @param progress Progress value (0-100)
 * @param line LCD line for progress bar
 */
void displayProgressBar(int progress, int line) {
  lcd.setCursor(0, line);
  lcd.print("[");
  
  int bars = (progress * 18) / 100; // 18 character progress bar
  for (int i = 0; i < 18; i++) {
    if (i < bars) {
      lcd.print("=");
    } else {
      lcd.print(" ");
    }
  }
  lcd.print("]");
}

// ==========================================
// DEBUG FUNCTIONS
// ==========================================

#if ENABLE_SERIAL_DEBUG
/**
 * Print current display mode to serial
 */
void debugPrintDisplayMode() {
  Serial.print("Current display mode: ");
  switch(currentDisplayMode) {
    case MODE_TEMP_HUMIDITY: Serial.println("Temperature/Humidity"); break;
    case MODE_FEELS_LIKE: Serial.println("Feels-like/Dew Point"); break;
    case MODE_HUMIDEX: Serial.println("Humidex"); break;
    case MODE_DATE_TIME: Serial.println("Date/Time"); break;
    case MODE_SYSTEM_INFO: Serial.println("System Info"); break;
    default: Serial.println("Unknown"); break;
  }
}
#endif

#endif // DISPLAY_UTILS_H