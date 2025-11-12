/**
 * @file storage.cpp
 * @brief EEPROM storage implementation
 * 
 * @author F. Baillon
 * @version 1.1.0
 * @date January 2025
 * @license MIT License
 */

#include "storage.h"


// ==========================================
// FUNCTION IMPLEMENTATIONS
// ==========================================

void initStorage() {
  DEBUG_PRINTLN("Initializing EEPROM storage...");
  
  ClockConfig config;
  
  if (loadConfig(&config)) {
    DEBUG_PRINTLN("Valid config loaded from EEPROM");
    applyConfig(&config);
  } else {
    DEBUG_PRINTLN("No valid config found, using defaults");
    createDefaultConfig(&config);
    saveConfig(&config);
    applyConfig(&config);
  }
}

bool loadConfig(ClockConfig* config) {
  // Read config from EEPROM
  EEPROM.get(EEPROM_CONFIG_ADDR, *config);
  
  // Validate magic number
  if (config->magic != CONFIG_MAGIC) {
    DEBUG_PRINTLN("Invalid magic number in EEPROM");
    return false;
  }
  
  // Validate checksum
  uint16_t savedChecksum = config->checksum;
  uint16_t calculatedChecksum = calculateChecksum(config);
  
  if (savedChecksum != calculatedChecksum) {
    DEBUG_PRINTLN("Checksum mismatch in EEPROM");
    DEBUG_PRINT("Saved: ");
    DEBUG_PRINT(savedChecksum);
    DEBUG_PRINT(", Calculated: ");
    DEBUG_PRINTLN(calculatedChecksum);
    return false;
  }
  
  return true;
}

bool saveConfig(const ClockConfig* config) {
  // Read existing config
  ClockConfig existingConfig;
  EEPROM.get(EEPROM_CONFIG_ADDR, existingConfig);
  
  // Compare (skip checksum field in comparison)
  if (memcmp(config, &existingConfig, sizeof(ClockConfig) - sizeof(uint16_t)) == 0) {
    DEBUG_PRINTLN("Config unchanged, skipping EEPROM write");
    return false;
  }
  
  // Create copy and update checksum
  ClockConfig configToSave = *config;
  configToSave.magic = CONFIG_MAGIC;
  configToSave.checksum = calculateChecksum(&configToSave);
  
  // Write to EEPROM
  EEPROM.put(EEPROM_CONFIG_ADDR, configToSave);
  
  DEBUG_PRINTLN("Config saved to EEPROM");
  return true;
}

void createDefaultConfig(ClockConfig* config) {
  config->magic = CONFIG_MAGIC;
  
  // WiFi from secrets.h
  strncpy(config->wifiSSID, ssid, 31);
  config->wifiSSID[31] = '\0';
  strncpy(config->wifiPassword, pass, 63);
  config->wifiPassword[63] = '\0';
  
  // NTP settings from config.h defaults
  config->timezoneOffset = TIME_ZONE_OFFSET;
  config->ntpSyncHour = NTP_SYNC_HOUR;
  config->ntpSyncMinute = NTP_SYNC_MINUTE;
  
  // LED colors from config.h defaults
  config->colorHourR = COLOR_HOUR_R;
  config->colorHourG = COLOR_HOUR_G;
  config->colorHourB = COLOR_HOUR_B;
  config->colorMinuteR = COLOR_MINUTE_R;
  config->colorMinuteG = COLOR_MINUTE_G;
  config->colorMinuteB = COLOR_MINUTE_B;
  config->colorSecondR = COLOR_SECOND_R;
  config->colorSecondG = COLOR_SECOND_G;
  config->colorSecondB = COLOR_SECOND_B;
  
  // LED brightness
  config->ledBrightness = 100;
  
  // LCD timeout
  config->lcdTimeout = LCD_BACKLIGHT_TIMEOUT;
  
  // Moon module (default values - will be updated by initMoon)
  config->nextNewMoonEpoch = 0;
  config->lastMeeusSync = 0;
  config->moonModuleEnabled = 1;  // Enabled by default
  
  // Checksum
  config->checksum = calculateChecksum(config);
  
  DEBUG_PRINTLN("Default config created");
}

uint16_t calculateChecksum(const ClockConfig* config) {
  uint16_t sum = 0;
  const uint8_t* data = (const uint8_t*)config;
  size_t size = sizeof(ClockConfig) - sizeof(uint16_t);  // Exclude checksum field
  
  for (size_t i = 0; i < size; i++) {
    sum += data[i];
  }
  
  return sum;
}

void applyConfig(const ClockConfig* config) {
  DEBUG_PRINTLN("Applying configuration...");
  
  // Apply LED brightness
  runtimeLedBrightness = config->ledBrightness;
  ledsHour.setBrightness(runtimeLedBrightness);
  ledsMinuteSec.setBrightness(runtimeLedBrightness);
  ledsAirQuality.setBrightness(runtimeLedBrightness);
  ledsHour.show();
  ledsMinuteSec.show();
  ledsAirQuality.show();
  DEBUG_PRINT("LED brightness set to: ");
  DEBUG_PRINTLN(runtimeLedBrightness);
  
  // Apply LED colors
  runtimeColorHourR = config->colorHourR;
  runtimeColorHourG = config->colorHourG;
  runtimeColorHourB = config->colorHourB;
  runtimeColorMinuteR = config->colorMinuteR;
  runtimeColorMinuteG = config->colorMinuteG;
  runtimeColorMinuteB = config->colorMinuteB;
  runtimeColorSecondR = config->colorSecondR;
  runtimeColorSecondG = config->colorSecondG;
  runtimeColorSecondB = config->colorSecondB;
  DEBUG_PRINTLN("LED colors updated");
  
  // Apply LCD timeout
  runtimeLcdTimeout = config->lcdTimeout;
  DEBUG_PRINT("LCD timeout set to: ");
  DEBUG_PRINT(runtimeLcdTimeout / 1000);
  DEBUG_PRINTLN(" seconds");

  // Apply timezone offset
  runtimeTimezoneOffset = config->timezoneOffset;
  timeClient.setTimeOffset(runtimeTimezoneOffset * 3600);
  DEBUG_PRINT("Timezone offset set to: UTC");
  if (runtimeTimezoneOffset >= 0) DEBUG_PRINT("+");
  DEBUG_PRINTLN(runtimeTimezoneOffset);
  
  // Apply NTP sync schedule
  runtimeNtpSyncHour = config->ntpSyncHour;
  runtimeNtpSyncMinute = config->ntpSyncMinute;
  DEBUG_PRINT("NTP sync schedule set to: ");
  if (runtimeNtpSyncHour < 10) DEBUG_PRINT("0");
  DEBUG_PRINT(runtimeNtpSyncHour);
  DEBUG_PRINT(":");
  if (runtimeNtpSyncMinute < 10) DEBUG_PRINT("0");
  DEBUG_PRINTLN(runtimeNtpSyncMinute);
  
  // Note: NTP settings require restart
  DEBUG_PRINTLN("Configuration applied successfully");
}

void getCurrentConfig(ClockConfig* config) {
  // Fill config with current runtime values
  // For now, we'll load from EEPROM as baseline
  if (!loadConfig(config)) {
    createDefaultConfig(config);
  }
  
  // Override with any runtime changes
  // (we'll implement this as we make settings changeable at runtime)
}