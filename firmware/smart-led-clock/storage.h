/**
 * @file storage.h
 * @brief EEPROM storage management for configuration
 * 
 * Handles persistent storage of configuration parameters in EEPROM.
 * Implements wear-leveling by only writing when values actually change.
 * 
 * EEPROM lifecycle:
 * - Capacity: 8 KB on Arduino Uno R4 WiFi
 * - Write cycles: 100,000 minimum per cell
 * - Expected lifetime: 274+ years at 1 write/day
 * 
 * @author F. Baillon
 * @version 1.1.0
 * @date January 2025
 * @license MIT License
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <EEPROM.h>
#include <string.h>
#include "config.h"
#include "secrets.h"
#include "leds.h"

// ==========================================
// CONFIGURATION STRUCTURE
// ==========================================

/**
 * @struct ClockConfig
 * @brief Configuration parameters stored in EEPROM
 * 
 * Total size: ~180 bytes (out of 8192 available)
 */
struct ClockConfig {
  // Magic number to validate EEPROM data
  uint16_t magic;              // 0xC10C (Clock)
  
  // WiFi settings
  char wifiSSID[32];           // WiFi network name
  char wifiPassword[64];       // WiFi password
  
  // NTP settings
  int8_t timezoneOffset;       // UTC offset in hours (-12 to +14)
  uint8_t ntpSyncHour;         // Hour for daily NTP sync (0-23)
  uint8_t ntpSyncMinute;       // Minute for daily NTP sync (0-59)
  
  // LED colors (RGB 0-255)
  uint8_t colorHourR;
  uint8_t colorHourG;
  uint8_t colorHourB;
  uint8_t colorMinuteR;
  uint8_t colorMinuteG;
  uint8_t colorMinuteB;
  uint8_t colorSecondR;
  uint8_t colorSecondG;
  uint8_t colorSecondB;
  
  // LED brightness (0-255)
  uint8_t ledBrightness;
  
  // LCD settings
  uint32_t lcdTimeout;         // Backlight timeout in milliseconds
  
  // Language
  uint8_t language;            // LANG_FR or LANG_EN
  
  // Debug mode
  uint8_t debugMode;           // 0 or 1
  
  // Moon phase tracking (Meeus hybrid approach)
  unsigned long nextNewMoonEpoch;   // Next new moon timestamp (Meeus calculated)
  unsigned long lastMeeusSync;      // Last Meeus synchronization timestamp
  uint8_t moonModuleEnabled;        // Moon module enabled flag (0 or 1)
  
  // Checksum for data integrity
  uint16_t checksum;
};

// EEPROM address for config (starting at 0)
#define EEPROM_CONFIG_ADDR 0

// Magic number to identify valid config
#define CONFIG_MAGIC 0xC10C

// ==========================================
// FUNCTION DECLARATIONS
// ==========================================

/**
 * @brief Initialize storage system
 * 
 * Loads configuration from EEPROM or creates default config
 * if EEPROM is empty or corrupted.
 */
void initStorage();

/**
 * @brief Load configuration from EEPROM
 * 
 * @param config Pointer to config structure to fill
 * @return true if valid config loaded, false if using defaults
 */
bool loadConfig(ClockConfig* config);

/**
 * @brief Save configuration to EEPROM
 * 
 * Only writes if configuration has changed (wear-leveling).
 * Updates checksum before writing.
 * 
 * @param config Pointer to config structure to save
 * @return true if saved successfully, false if unchanged or error
 */
bool saveConfig(const ClockConfig* config);

/**
 * @brief Create default configuration
 * 
 * @param config Pointer to config structure to fill with defaults
 */
void createDefaultConfig(ClockConfig* config);

/**
 * @brief Calculate checksum for config structure
 * 
 * Simple checksum: sum of all bytes except checksum field.
 * 
 * @param config Pointer to config structure
 * @return Calculated checksum value
 */
uint16_t calculateChecksum(const ClockConfig* config);

/**
 * @brief Apply configuration to running system
 * 
 * Updates all system parameters from config structure.
 * Call this after loading config from EEPROM.
 * 
 * @param config Pointer to config to apply
 */
void applyConfig(const ClockConfig* config);

/**
 * @brief Get current running configuration
 * 
 * Fills config structure with current system values.
 * 
 * @param config Pointer to config structure to fill
 */
void getCurrentConfig(ClockConfig* config);

#endif // STORAGE_H