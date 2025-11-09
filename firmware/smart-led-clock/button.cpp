/**
 * @file button.cpp
 * @brief Button input management implementation
 * 
 * @author F. Baillon
 * @version 1.0.0
 * @date January 2025
 * @license MIT License
 * 
 * Copyright (c) 2025 F. Baillon
 */

#include "button.h"

// ==========================================
// GLOBAL BUTTON OBJECT
// ==========================================
OneButton button;

// ==========================================
// FUNCTION IMPLEMENTATIONS
// ==========================================

/**
 * @brief Initialize button with callbacks
 * 
 * Configures the button pin with internal pull-up resistor
 * and attaches callback functions for different press types.
 * 
 * Button is configured as active LOW (pressed = LOW signal).
 * Uses OneButton library for debouncing and press detection.
 */
void initButton() {
  button.setup(PIN_BUTTON, INPUT_PULLUP, true);
  button.attachClick(buttonClick);
  button.attachLongPressStop(buttonLongPress);
  
  DEBUG_PRINTLN("Button initialized on pin " + String(PIN_BUTTON));
}

/**
 * @brief Callback for button single click
 * 
 * Behavior depends on LCD backlight state:
 * - If LCD is OFF: Turn on backlight only (no mode change)
 * - If LCD is ON: Cycle to next display mode
 * 
 * Cycling order:
 * 1. MODE_TEMP_HUMIDITY (default)
 * 2. MODE_FEELS_LIKE
 * 3. MODE_HUMIDEX
 * Then loops back to MODE_TEMP_HUMIDITY
 * 
 * Updates lastLCDActivity to reset auto-off timer.
 */
void buttonClick() {
  lastLCDActivity = millis();
  
  // If LCD is off, just wake it up (no mode change)
  if (!lcdBacklightOn) {
    wakeUpLCD();
    DEBUG_PRINTLN("LCD backlight ON (wake up)");
    return;
  }
  
  // Cycle to next mode
  currentDisplayMode = (DisplayMode)((currentDisplayMode + 1) % MODE_COUNT);
  DEBUG_PRINT("Mode changed to: ");
  DEBUG_PRINTLN(currentDisplayMode);
  clearLCD();
}

/**
 * @brief Callback for button long press
 * 
 * Returns to default display mode (MODE_TEMP_HUMIDITY).
 * If LCD backlight is off, turns it on first.
 * 
 * Long press duration: >2 seconds (OneButton default)
 * 
 * Updates lastLCDActivity to reset auto-off timer.
 */
void buttonLongPress() {
  lastLCDActivity = millis();
  
  // Wake LCD if off
  if (!lcdBacklightOn) {
    wakeUpLCD();
    DEBUG_PRINTLN("LCD backlight ON (long press)");
  }
  
  // Return to default mode
  currentDisplayMode = MODE_TEMP_HUMIDITY;
  DEBUG_PRINTLN("Returning to default mode");
  clearLCD();
}

/**
 * @brief Update button state (call in main loop)
 * 
 * Must be called frequently (every loop iteration) to ensure
 * proper button debouncing and press detection.
 * 
 * The OneButton library internally handles:
 * - Debouncing
 * - Click detection
 * - Long press detection
 * - Double click (not used in this project)
 */
void updateButton() {
  button.tick();
}