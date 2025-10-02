/**
 * Smart LED Clock - Display Module Implementation
 * 
 * Author: F. Baillon
 * Version: Phase 5
 * Date: January 2025
 * License: GPL v3.0
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

void initDisplay() {
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, degreeSymbol);
  lcd.clear();
  
  Serial.println("LCD initialized");
}

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

void displayTempHumidity(DateTime now) {
  static char lastDateBuffer[21] = "";
  static char lastTimeBuffer[21] = "";
  static char lastIndoorBuffer[21] = "";
  static char lastOutdoorBuffer[21] = "";
  
  // Line 0: Date + Day
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

  // Line 1: Time
  char timeBuffer[21];
  sprintf(timeBuffer, "Heure: %02d:%02d:%02d   ", now.hour(), now.minute(), now.second());
  if (strcmp(timeBuffer, lastTimeBuffer) != 0) {
    lcd.setCursor(0, 1);
    lcd.print(timeBuffer);
    strcpy(lastTimeBuffer, timeBuffer);
  }

  // Line 2: Indoor
  char indoorBuffer[21];
  if (indoorData.valid) {
    sprintf(indoorBuffer, "Int:%4.1f%cC   %3d%%  ", 
            indoorData.temperature, 
            223,
            (int)indoorData.humidity);
  } else {
    sprintf(indoorBuffer, "Int: ERREUR         ");
  }
  
  if (strcmp(indoorBuffer, lastIndoorBuffer) != 0) {
    lcd.setCursor(0, 2);
    lcd.print("Int:");
    if (indoorData.valid) {
      lcd.print(indoorData.temperature, 1);
      lcd.write(byte(0));
      lcd.print("C   ");
      lcd.print((int)indoorData.humidity);
      lcd.print("%  ");
    } else {
      lcd.print(" ERREUR         ");
    }
    strcpy(lastIndoorBuffer, indoorBuffer);
  }

  // Line 3: Outdoor + AQI
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
      lcd.write(byte(0));
      lcd.print(" AQI:");
      lcd.print(airQuality.estimatedAQI);
      lcd.print("   ");
    } else {
      lcd.print("ERR  AQI:---    ");
    }
    strcpy(lastOutdoorBuffer, outdoorBuffer);
  }
}

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

  if (outdoorData.valid && abs(outdoorData.humidex - lastHumidex) >= 2) {
    lcd.setCursor(0, 2);
    const char* desc = getHumidexDescription(outdoorData.humidex);
    lcd.print(desc);
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

const char* getHumidexDescription(int humidex) {
  if (humidex < 20) return "Pas d'inconfort";
  if (humidex < 30) return "Peu d'inconfort";
  if (humidex < 40) return "Inconfort certain";
  if (humidex < 45) return "Eviter efforts";
  return "Danger chaleur";
}

void displayStartupMessage(const char* message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart LED Clock     ");
  lcd.setCursor(0, 1);
  lcd.print("Phase 5 - Final     ");
  lcd.setCursor(0, 3);
  lcd.print(message);
  int msgLen = strlen(message);
  for (int i = msgLen; i < LCD_COLUMNS; i++) {
    lcd.print(" ");
  }
}

void showAnimationMessage() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("  Animation horaire ");
}

void wakeUpLCD() {
  lcd.backlight();
  lcdBacklightOn = true;
  lcd.clear();
}

void clearLCD() {
  lcd.clear();
}

void manageLCDBacklight() {
  if (lcdBacklightOn && (millis() - lastLCDActivity > LCD_BACKLIGHT_TIMEOUT)) {
    lcd.noBacklight();
    lcdBacklightOn = false;
    Serial.println("LCD backlight OFF");
  }
}