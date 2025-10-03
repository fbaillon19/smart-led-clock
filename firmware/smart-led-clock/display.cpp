/**
 * @file display.cpp
 * @brief LCD display management implementation
 * 
 * @author F. Baillon
 * @version 1.0.0
 * @date January 2025
 * @license MIT License
 * 
 * Copyright (c) 2025 F. Baillon
 */

#include "display.h"
#include <Arduino.h>

// ==========================================
// GLOBAL LCD OBJECT
// ==========================================
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// Custom degree symbol for LCD
byte degreeSymbol[8] = {
  0b01100, 0b10010, 0b10010, 0b01100,
  0b00000, 0b00000, 0b00000, 0b00000
};

// ==========================================
// FUNCTION IMPLEMENTATIONS
// ==========================================

/**
 * @brief Initialize LCD display
 * 
 * Performs LCD initialization:
 * - Initializes I2C communication
 * - Turns on backlight
 * - Creates custom degree symbol (°) character
 * - Clears display
 * 
 * The custom degree symbol is stored in LCD memory slot 0
 * and can be displayed using lcd.write(byte(0)).
 */
void initDisplay() {
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, degreeSymbol);
  lcd.clear();
  
  Serial.println("LCD initialized");
}

/**
 * @brief Update LCD display based on current mode
 * 
 * Routes display update to appropriate mode function:
 * - MODE_TEMP_HUMIDITY → displayTempHumidity()
 * - MODE_FEELS_LIKE → displayFeelsLike()
 * - MODE_HUMIDEX → displayHumidex()
 * 
 * Called every 2 seconds when LCD backlight is on.
 * 
 * @param now Current DateTime from RTC
 */
void updateLCDDisplay(DateTime now) {
  switch (currentDisplayMode) {
    case MODE_TEMP_HUMIDITY:
      displayTempHumidity(now);
      break;
    case MODE_FEELS_LIKE:
      displayFeelsLike(now);
      break;
    case MODE_HUMIDEX:
      displayHumidex(now);
      break;
  }
}

/**
 * @brief Display Mode 1: Temperature & Humidity
 * 
 * Shows comprehensive environmental data:
 * - Line 0: Date (DD/MM/YYYY) and day of week (French)
 * - Line 1: Time (HH:MM:SS)
 * - Line 2: Indoor temperature (°C) and humidity (%)
 * - Line 3: Outdoor temperature (°C) and Air Quality Index
 * 
 * Optimization: Uses static buffers to track last displayed values.
 * Only updates LCD when data changes, reducing I2C traffic and flicker.
 * 
 * Example display:
 * ```
 * 03/10/2025        Sam
 * Heure: 14:32:45
 * Int:22.5°C   65%
 * Ext:18.3°C AQI:42
 * ```
 * 
 * @param now Current DateTime from RTC
 */
void displayTempHumidity(DateTime now) {
  static char lastDateBuffer[21] = "";
  static char lastTimeBuffer[21] = "";
  static char lastIndoorBuffer[21] = "";
  static char lastOutdoorBuffer[21] = "";
  
  // Line 0: Date + Day (only update if changed)
  char dateBuffer[21];
  sprintf(dateBuffer, "%02d/%02d/%04d", now.day(), now.month(), now.year());
  const char* days[] = {"Dim", "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam"};
  char fullDateBuffer[21];
  sprintf(fullDateBuffer, "%s        %s", dateBuffer, days[now.dayOfTheWeek()]);
  
  if (strcmp(fullDateBuffer, lastDateBuffer) != 0) {
    lcd.setCursor(0, 0);
    lcd.print(fullDateBuffer);
    strcpy(lastDateBuffer, fullDateBuffer);
  }

  // Line 1: Time (always update for seconds)
  char timeBuffer[21];
  sprintf(timeBuffer, "Heure: %02d:%02d:%02d   ", now.hour(), now.minute(), now.second());
  if (strcmp(timeBuffer, lastTimeBuffer) != 0) {
    lcd.setCursor(0, 1);
    lcd.print(timeBuffer);
    strcpy(lastTimeBuffer, timeBuffer);
  }

  // Line 2: Indoor (only update if data changed)
  char indoorBuffer[21];
  if (indoorData.valid) {
    sprintf(indoorBuffer, "Int:%4.1f%cC   %3d%%  ", 
            indoorData.temperature, 
            223, // degree symbol placeholder for comparison
            (int)indoorData.humidity);
  } else {
    sprintf(indoorBuffer, "Int: ERREUR         ");
  }
  
  if (strcmp(indoorBuffer, lastIndoorBuffer) != 0) {
    lcd.setCursor(0, 2);
    lcd.print("Int:");
    if (indoorData.valid) {
      lcd.print(indoorData.temperature, 1);
      lcd.write(byte(0)); // Custom degree symbol
      lcd.print("C   ");
      lcd.print((int)indoorData.humidity);
      lcd.print("%  ");
    } else {
      lcd.print(" ERREUR         ");
    }
    strcpy(lastIndoorBuffer, indoorBuffer);
  }

  // Line 3: Outdoor + AQI (only update if data changed)
  char outdoorBuffer[21];
  if (outdoorData.valid) {
    sprintf(outdoorBuffer, "Ext:%4.1f AQI:%3d", 
            outdoorData.temperature, 
            airQuality.estimatedAQI);
  } else {
    sprintf(outdoorBuffer, "Ext:ERR  AQI:---    ");
  }
  
  if (strcmp(outdoorBuffer, lastOutdoorBuffer) != 0) {
    lcd.setCursor(0, 3);
    lcd.print("Ext:");
    if (outdoorData.valid) {
      lcd.print(outdoorData.temperature, 1);
      lcd.write(byte(0)); // Custom degree symbol
      lcd.print(" AQI:");
      lcd.print(airQuality.estimatedAQI);
      lcd.print("   ");
    } else {
      lcd.print("ERR  AQI:---    ");
    }
    strcpy(lastOutdoorBuffer, outdoorBuffer);
  }
}

/**
 * @brief Display Mode 2: Feels-like Temperature
 * 
 * Shows outdoor temperature details:
 * - Line 0: Mode title
 * - Line 1: Actual outdoor temperature
 * - Line 2: Feels-like temperature (heat index)
 * - Line 3: Dew point temperature
 * 
 * All values from outdoor sensor only.
 * Displays "ERREUR" if outdoor sensor data is invalid.
 * 
 * Example display:
 * ```
 * Temp. Ressentie
 * Exterieur: 18.3°C
 * Ressenti : 17.1°C
 * Pt rosee : 12.4°C
 * ```
 * 
 * @param now Current DateTime from RTC (unused but kept for consistency)
 */
void displayFeelsLike(DateTime now) {
  lcd.setCursor(0, 0);
  lcd.print("Temp. Ressentie     ");

  lcd.setCursor(0, 1);
  if (outdoorData.valid) {
    lcd.print("Exterieur: ");
    lcd.print(outdoorData.temperature, 1);
    lcd.write(byte(0));
    lcd.print("C  ");
  } else {
    lcd.print("Exterieur: ERREUR   ");
  }

  lcd.setCursor(0, 2);
  if (outdoorData.valid) {
    lcd.print("Ressenti : ");
    lcd.print(outdoorData.feelsLike, 1);
    lcd.write(byte(0));
    lcd.print("C  ");
  } else {
    lcd.print("Ressenti : ERREUR   ");
  }

  lcd.setCursor(0, 3);
  if (outdoorData.valid) {
    lcd.print("Pt rosee : ");
    lcd.print(outdoorData.dewPoint, 1);
    lcd.write(byte(0));
    lcd.print("C  ");
  } else {
    lcd.print("Pt rosee : ERREUR   ");
  }
}

/**
 * @brief Display Mode 3: Humidex Index
 * 
 * Shows Canadian Humidex comfort index:
 * - Line 0: Mode title
 * - Line 1: Humidex value (unitless index)
 * - Line 2: Comfort description (French)
 * - Line 3: Note that it's outdoor only
 * 
 * Humidex descriptions:
 * - < 20: "Pas d'inconfort"
 * - 20-29: "Peu d'inconfort"
 * - 30-39: "Inconfort certain"
 * - 40-44: "Eviter efforts"
 * - ≥ 45: "Danger chaleur"
 * 
 * Only updates description when humidex changes by ±2 or more
 * to reduce unnecessary LCD updates.
 * 
 * Example display:
 * ```
 * Indice Humidex
 * Humidex:        23
 * Peu d'inconfort
 * Exterieur uniquement
 * ```
 * 
 * @param now Current DateTime from RTC (unused but kept for consistency)
 */
void displayHumidex(DateTime now) {
  static int lastHumidex = -999;
  
  lcd.setCursor(0, 0);
  lcd.print("Indice Humidex      ");

  lcd.setCursor(0, 1);
  if (outdoorData.valid) {
    lcd.print("Humidex:        ");
    lcd.setCursor(9, 1);
    lcd.print(outdoorData.humidex);
    lcd.print("  ");
  } else {
    lcd.print("Humidex: ERREUR     ");
  }

  // Only update description if humidex changed significantly (±2)
  if (outdoorData.valid && abs(outdoorData.humidex - lastHumidex) >= 2) {
    lcd.setCursor(0, 2);
    const char* desc = getHumidexDescription(outdoorData.humidex);
    lcd.print(desc);
    // Pad remaining line with spaces
    int len = strlen(desc);
    for (int i = len; i < LCD_COLUMNS; i++) {
      lcd.print(" ");
    }
    lastHumidex = outdoorData.humidex;
  } else if (!outdoorData.valid) {
    lcd.setCursor(0, 2);
    lcd.print("                    ");
  }

  lcd.setCursor(0, 3);
  lcd.print("Exterieur uniquement");
}

/**
 * @brief Get humidex comfort description
 * 
 * Converts humidex value to French comfort description.
 * 
 * Scale (Canadian standard):
 * - < 20: No discomfort
 * - 20-29: Little discomfort
 * - 30-39: Some discomfort
 * - 40-44: Great discomfort, avoid exertion
 * - ≥ 45: Dangerous, heat stroke risk
 * 
 * @param humidex Humidex value (unitless index)
 * @return Pointer to description string (French)
 */
const char* getHumidexDescription(int humidex) {
  if (humidex < 20) return "Pas d'inconfort";
  if (humidex < 30) return "Peu d'inconfort";
  if (humidex < 40) return "Inconfort certain";
  if (humidex < 45) return "Eviter efforts";
  return "Danger chaleur";
}

/**
 * @brief Display startup message
 * 
 * Shows standardized startup screen with:
 * - Line 0: "Smart LED Clock"
 * - Line 1: "Phase 5 - Final"
 * - Line 2: (empty)
 * - Line 3: Custom message (padded to full line)
 * 
 * Used during initialization to show progress.
 * 
 * @param message Status message to display on line 3
 */
void displayStartupMessage(const char* message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart LED Clock     ");
  lcd.setCursor(0, 1);
  lcd.print("Phase 5 - Final     ");
  lcd.setCursor(0, 3);
  lcd.print(message);
  // Pad message line with spaces to clear any previous text
  int msgLen = strlen(message);
  for (int i = msgLen; i < LCD_COLUMNS; i++) {
    lcd.print(" ");
  }
}

/**
 * @brief Show animation message on LCD
 * 
 * Displays "Animation horaire" message during hourly animation.
 * Centers text on line 1 with padding.
 */
void showAnimationMessage() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("  Animation horaire ");
}

/**
 * @brief Wake up LCD (turn on backlight)
 * 
 * Turns on LCD backlight and sets flag.
 * Clears display to prepare for new content.
 * Resets auto-off timer (handled by caller updating lastLCDActivity).
 */
void wakeUpLCD() {
  lcd.backlight();
  lcdBacklightOn = true;
  lcd.clear();
}

/**
 * @brief Clear LCD display
 * 
 * Simple wrapper for lcd.clear().
 * Used for consistency across the codebase.
 */
void clearLCD() {
  lcd.clear();
}

/**
 * @brief Manage LCD backlight timeout
 * 
 * Automatically turns off LCD backlight after LCD_BACKLIGHT_TIMEOUT
 * milliseconds of inactivity (default: 30 seconds).
 * 
 * Inactivity is tracked via lastLCDActivity, which is updated on:
 * - Button press
 * - Mode change
 * - Any user interaction
 * 
 * Call this function in main loop to enable auto-off feature.
 * When backlight turns off, a message is printed to Serial.
 */
void manageLCDBacklight() {
  if (lcdBacklightOn && (millis() - lastLCDActivity > LCD_BACKLIGHT_TIMEOUT)) {
    lcd.noBacklight();
    lcdBacklightOn = false;
    Serial.println("LCD backlight OFF");
  }
}