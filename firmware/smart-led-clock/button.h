/**
 * Smart LED Clock - Button Management
 * 
 * This file handles physical button interactions including:
 * - Button initialization and setup
 * - Single click, double click, and long press detection
 * - Display mode switching via button presses
 * - Button callback functions
 * 
 * Hardware: 2 push buttons connected to digital pins with internal pull-up
 * 
 * Author: F. Baillon
 * Version: 1.0
 * License: MIT
 */

#ifndef BUTTON_H
#define BUTTON_H

#include <OneButton.h>
#include "config.h"

// ==========================================
// FORWARD DECLARATIONS
// ==========================================

// Forward declarations of callback functions (defined later)
void onButton1Click();
void onButton1DoubleClick();
void onButton1LongPress();
void onButton2Click();
void onButton2DoubleClick();
void onButton2LongPress();

// Forward declarations of display functions (from display_utils.h)
void setTemporaryDisplay(int mode, unsigned long duration);
void togglePermanentFeelsLike();

// ==========================================
// GLOBAL VARIABLES
// ==========================================
// Button objects
extern OneButton button1;
extern OneButton button2;

// Button state tracking
extern bool button1Pressed;
extern bool button2Pressed;
extern unsigned long lastButtonAction;

// Display modes (from display_utils.h)
extern int currentDisplayMode;
extern bool isTemporaryDisplay;
extern bool isPermanentFeelsLike;

// Constants from config.h
extern const int MODE_FEELS_LIKE;
extern const int MODE_HUMIDEX;
extern const int MODE_DATE_TIME;
extern const int MODE_SYSTEM_INFO;
extern const unsigned long DISPLAY_TIMEOUT;

// ==========================================
// INITIALIZATION FUNCTIONS
// ==========================================

/**
 * Initialize both buttons with callbacks
 * Sets up pull-up resistors and event handlers
 */
void initButtons() {
  // Initialize button 1
  button1.setup(PIN_BUTTON_1, INPUT_PULLUP, true);
  button1.attachClick(onButton1Click);
  button1.attachDoubleClick(onButton1DoubleClick);
  button1.attachLongPressStop(onButton1LongPress);
  button1.setDebounceTicks(50);     // 50ms debounce
  button1.setClickTicks(400);       // 400ms max for single click
  button1.setPressTicks(1000);      // 1000ms for long press
  
  // Initialize button 2
  button2.setup(PIN_BUTTON_2, INPUT_PULLUP, true);
  button2.attachClick(onButton2Click);
  button2.attachDoubleClick(onButton2DoubleClick);
  button2.attachLongPressStop(onButton2LongPress);
  button2.setDebounceTicks(50);     // 50ms debounce
  button2.setClickTicks(400);       // 400ms max for single click
  button2.setPressTicks(1000);      // 1000ms for long press
  
  lastButtonAction = millis();
}

/**
 * Process button inputs - call this regularly from main loop
 */
void processButtons() {
  button1.tick();
  button2.tick();
}

// ==========================================
// BUTTON CALLBACK FUNCTIONS IMPLEMENTATION
// ==========================================

/**
 * Button 1 single click handler
 * Temporarily shows feels-like temperature and dew point
 */
void onButton1Click() {
  lastButtonAction = millis();
  button1Pressed = true;
  
  // Switch to feels-like temperature display temporarily
  // Note: These constants need to be defined in config.h or display_utils.h
  setTemporaryDisplay(1, 4000); // MODE_FEELS_LIKE, 4 seconds
  
#if ENABLE_SERIAL_DEBUG
  Serial.println("Button 1 click: Showing feels-like temperature");
#endif
}

/**
 * Button 1 double click handler
 * Shows humidex display temporarily
 */
void onButton1DoubleClick() {
  lastButtonAction = millis();
  
  // Switch to humidex display temporarily
  setTemporaryDisplay(2, 4000); // MODE_HUMIDEX, 4 seconds
  
#if ENABLE_SERIAL_DEBUG
  Serial.println("Button 1 double click: Showing humidex");
#endif
}

/**
 * Button 1 long press handler
 * Toggles permanent feels-like display mode
 */
void onButton1LongPress() {
  lastButtonAction = millis();
  
  // Toggle permanent feels-like mode
  togglePermanentFeelsLike();
  
#if ENABLE_SERIAL_DEBUG
  Serial.print("Button 1 long press: Permanent feels-like mode ");
  Serial.println(isPermanentFeelsLike ? "ON" : "OFF");
#endif
}

/**
 * Button 2 single click handler
 * Shows date/time temporarily
 */
void onButton2Click() {
  lastButtonAction = millis();
  button2Pressed = true;
  
  // Switch to date/time display temporarily
  setTemporaryDisplay(3, 4000); // MODE_DATE_TIME, 4 seconds
  
#if ENABLE_SERIAL_DEBUG
  Serial.println("Button 2 click: Showing date/time");
#endif
}

/**
 * Button 2 double click handler
 * Shows system information temporarily
 */
void onButton2DoubleClick() {
  lastButtonAction = millis();
  
  // Switch to system info display temporarily
  setTemporaryDisplay(4, 4000); // MODE_SYSTEM_INFO, 4 seconds
  
#if ENABLE_SERIAL_DEBUG
  Serial.println("Button 2 double click: Showing system info");
#endif
}

/**
 * Button 2 long press handler
 * Reserved for future functionality
 */
void onButton2LongPress() {
  lastButtonAction = millis();
  
  // Future functionality - for now, show system info longer
  setTemporaryDisplay(4, 8000); // MODE_SYSTEM_INFO, 8 seconds
  
#if ENABLE_SERIAL_DEBUG
  Serial.println("Button 2 long press: Extended system info");
#endif
}

// ==========================================
// BUTTON STATE FUNCTIONS
// ==========================================

/**
 * Check if any button was recently pressed
 * @param timeoutMs Time in milliseconds to consider "recent"
 * @return true if button was pressed within timeout
 */
bool wasButtonRecentlyPressed(unsigned long timeoutMs) {
  return (millis() - lastButtonAction) < timeoutMs;
}

/**
 * Get time since last button action
 * @return milliseconds since last button press
 */
unsigned long getTimeSinceLastButtonAction() {
  return millis() - lastButtonAction;
}

/**
 * Reset button action timer
 * Useful for external events that should reset timeout
 */
void resetButtonActionTimer() {
  lastButtonAction = millis();
}

/**
 * Check if button 1 is currently pressed
 * @return true if button 1 is physically pressed
 */
bool isButton1Pressed() {
  return !digitalRead(PIN_BUTTON_1); // LOW when pressed (pull-up)
}

/**
 * Check if button 2 is currently pressed
 * @return true if button 2 is physically pressed
 */
bool isButton2Pressed() {
  return !digitalRead(PIN_BUTTON_2); // LOW when pressed (pull-up)
}

// ==========================================
// BUTTON CONFIGURATION
// ==========================================

/**
 * Set button timing parameters
 * @param debounceTicks Debounce time in milliseconds
 * @param clickTicks Maximum time for single click
 * @param pressTicks Minimum time for long press
 */
void configureButtonTiming(int debounceTicks, int clickTicks, int pressTicks) {
  button1.setDebounceTicks(debounceTicks);
  button1.setClickTicks(clickTicks);
  button1.setPressTicks(pressTicks);
  
  button2.setDebounceTicks(debounceTicks);
  button2.setClickTicks(clickTicks);
  button2.setPressTicks(pressTicks);
}

/**
 * Disable button processing temporarily
 * Useful during animations or critical operations
 */
void disableButtons() {
  // Note: OneButton library doesn't have direct disable
  // We can implement this by not calling tick() or checking a flag
  // For now, this is a placeholder for future implementation
}

/**
 * Re-enable button processing
 */
void enableButtons() {
  // Corresponding enable function
  // For now, this is a placeholder for future implementation
}

// ==========================================
// DEBUG FUNCTIONS
// ==========================================

#if ENABLE_SERIAL_DEBUG
/**
 * Print button status for debugging
 */
void debugPrintButtonStatus() {
  Serial.println("=== Button Status ===");
  Serial.print("Button 1 state: ");
  Serial.println(isButton1Pressed() ? "PRESSED" : "RELEASED");
  Serial.print("Button 2 state: ");
  Serial.println(isButton2Pressed() ? "PRESSED" : "RELEASED");
  Serial.print("Last action: ");
  Serial.print(getTimeSinceLastButtonAction());
  Serial.println(" ms ago");
  Serial.println("====================");
}

/**
 * Test button functionality
 * Call this to verify buttons are working
 */
void testButtons() {
  Serial.println("Button test mode - press buttons to verify");
  Serial.println("Button 1: Single=Feels-like, Double=Humidex, Long=Toggle");
  Serial.println("Button 2: Single=Date/Time, Double=System, Long=Extended");
  
  for (int i = 0; i < 100; i++) { // Test for 10 seconds
    processButtons();
    delay(100);
    
    if (i % 50 == 0) {
      Serial.print(".");
    }
  }
  Serial.println("\nButton test complete");
}
#endif

#endif // BUTTON_H

// ==========================================
// BUTTON STATE FUNCTIONS
// ==========================================

/**
 * Check if any button was recently pressed
 * @param timeoutMs Time in milliseconds to consider "recent"
 * @return true if button was pressed within timeout
 */
bool wasButtonRecentlyPressed(unsigned long timeoutMs) {
  return (millis() - lastButtonAction) < timeoutMs;
}

/**
 * Get time since last button action
 * @return milliseconds since last button press
 */
unsigned long getTimeSinceLastButtonAction() {
  return millis() - lastButtonAction;
}

/**
 * Reset button action timer
 * Useful for external events that should reset timeout
 */
void resetButtonActionTimer() {
  lastButtonAction = millis();
}

/**
 * Check if button 1 is currently pressed
 * @return true if button 1 is physically pressed
 */
bool isButton1Pressed() {
  return !digitalRead(PIN_BUTTON_1); // LOW when pressed (pull-up)
}

/**
 * Check if button 2 is currently pressed
 * @return true if button 2 is physically pressed
 */
bool isButton2Pressed() {
  return !digitalRead(PIN_BUTTON_2); // LOW when pressed (pull-up)
}

// ==========================================
// BUTTON CONFIGURATION
// ==========================================

/**
 * Set button timing parameters
 * @param debounceTicks Debounce time in milliseconds
 * @param clickTicks Maximum time for single click
 * @param pressTicks Minimum time for long press
 */
void configureButtonTiming(int debounceTicks, int clickTicks, int pressTicks) {
  button1.setDebounceTicks(debounceTicks);
  button1.setClickTicks(clickTicks);
  button1.setPressTicks(pressTicks);
  
  button2.setDebounceTicks(debounceTicks);
  button2.setClickTicks(clickTicks);
  button2.setPressTicks(pressTicks);
}

/**
 * Disable button processing temporarily
 * Useful during animations or critical operations
 */
void disableButtons() {
  // Note: OneButton library doesn't have direct disable
  // We can implement this by not calling tick() or checking a flag
  // For now, this is a placeholder for future implementation
}

/**
 * Re-enable button processing
 */
void enableButtons() {
  // Corresponding enable function
  // For now, this is a placeholder for future implementation
}

// ==========================================
// DEBUG FUNCTIONS
// ==========================================

#if ENABLE_SERIAL_DEBUG
/**
 * Print button status for debugging
 */
void debugPrintButtonStatus() {
  Serial.println("=== Button Status ===");
  Serial.print("Button 1 state: ");
  Serial.println(isButton1Pressed() ? "PRESSED" : "RELEASED");
  Serial.print("Button 2 state: ");
  Serial.println(isButton2Pressed() ? "PRESSED" : "RELEASED");
  Serial.print("Last action: ");
  Serial.print(getTimeSinceLastButtonAction());
  Serial.println(" ms ago");
  Serial.println("====================");
}

/**
 * Test button functionality
 * Call this to verify buttons are working
 */
void testButtons() {
  Serial.println("Button test mode - press buttons to verify");
  Serial.println("Button 1: Single=Feels-like, Double=Humidex, Long=Toggle");
  Serial.println("Button 2: Single=Date/Time, Double=System, Long=Extended");
  
  for (int i = 0; i < 100; i++) { // Test for 10 seconds
    processButtons();
    delay(100);
    
    if (i % 50 == 0) {
      Serial.print(".");
    }
  }
  Serial.println("\nButton test complete");
}
#endif // BUTTON_H