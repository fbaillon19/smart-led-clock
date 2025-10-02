/**
 * Smart LED Clock - Button Module Implementation
 * 
 * Author: F. Baillon
 * Version: Phase 5
 * Date: January 2025
 * License: GPL v3.0
 */

#include "button.h"
#include "display.h"
#include <Arduino.h>

// ==========================================
// GLOBAL BUTTON OBJECT
// ==========================================
OneButton button;

// ==========================================
// FUNCTION IMPLEMENTATIONS
// ==========================================

void initButton() {
  button.setup(PIN_BUTTON, INPUT_PULLUP, true);
  button.attachClick(buttonClick);
  button.attachLongPressStop(buttonLongPress);
  
  Serial.println("Button initialized on pin " + String(PIN_BUTTON));
}

void buttonClick() {
  lastLCDActivity = millis();
  
  // If LCD is off, just wake it up (no mode change)
  if (!lcdBacklightOn) {
    wakeUpLCD();
    Serial.println("LCD backlight ON (wake up)");
    return;
  }
  
  // Cycle to next mode
  currentDisplayMode = (DisplayMode)((currentDisplayMode + 1) % MODE_COUNT);
  Serial.print("Mode changed to: ");
  Serial.println(currentDisplayMode);
  clearLCD();
}

void buttonLongPress() {
  lastLCDActivity = millis();
  
  // Wake LCD if off
  if (!lcdBacklightOn) {
    wakeUpLCD();
    Serial.println("LCD backlight ON (long press)");
  }
  
  // Return to default mode
  currentDisplayMode = MODE_TEMP_HUMIDITY;
  Serial.println("Returning to default mode");
  clearLCD();
}

void updateButton() {
  button.tick();
}