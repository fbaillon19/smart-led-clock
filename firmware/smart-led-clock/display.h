/**
 * @file display.h
 * @brief LCD display management module
 * 
 * Handles all LCD display operations including multiple display modes,
 * automatic backlight management, and optimized screen updates.
 * 
 * Display modes:
 * - MODE_TEMP_HUMIDITY: Temperature and humidity display (default)
 * - MODE_FEELS_LIKE: Feels-like temperature and dew point
 * - MODE_HUMIDEX: Humidex comfort index
 * 
 * Features:
 * - Automatic backlight timeout (configurable)
 * - Optimized updates (only refresh changed data)
 * - Custom degree symbol character
 * - Startup messages
 * 
 * @author F. Baillon
 * @version 1.0.0
 * @date January 2025
 * @license MIT License
 * 
 * Copyright (c) 2025 F. Baillon
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include "config.h"

// ==========================================
// LCD OBJECT
// ==========================================
extern LiquidCrystal_I2C lcd;

// ==========================================
// FUNCTION DECLARATIONS
// ==========================================

/**
 * Initialize LCD display
 */
void initDisplay();

/**
 * Update LCD display based on current mode
 * @param now Current DateTime
 */
void updateLCDDisplay(DateTime now);

/**
 * Display Mode 1: Temperature & Humidity
 * @param now Current DateTime
 */
void displayTempHumidity(DateTime now);

/**
 * Display Mode 2: Feels-like & Dew point
 * @param now Current DateTime
 */
void displayFeelsLike(DateTime now);

/**
 * Display Mode 3: Humidex
 * @param now Current DateTime
 */
void displayHumidex(DateTime now);

/**
 * Get humidex description text
 * @param humidex Humidex value
 * @return Description string
 */
const char* getHumidexDescription(int humidex);

/**
 * Display startup message
 * @param message Message to display
 */
void displayStartupMessage(const char* message);

/**
 * Show animation message on LCD
 */
void showAnimationMessage();

/**
 * Wake up LCD (turn on backlight)
 */
void wakeUpLCD();

/**
 * Clear LCD display
 */
void clearLCD();

/**
 * Manage LCD backlight timeout
 */
void manageLCDBacklight();

#endif // DISPLAY_H