/**
 * @file button.h
 * @brief Button input management module
 * 
 * Handles button input with debouncing and multiple press types.
 * Uses the OneButton library for reliable button detection.
 * 
 * Button actions:
 * - Short press (LCD off): Wake up LCD backlight
 * - Short press (LCD on): Cycle through display modes
 * - Long press (>2s): Return to default display mode
 * 
 * @author F. Baillon
 * @version 1.1.0
 * @date November 2025
 * @license MIT License
 * 
 * Copyright (c) 2025 F. Baillon
 */

#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include <OneButton.h>
#include <LiquidCrystal_I2C.h>
#include "config.h"
#include "display.h"


// ==========================================
// BUTTON OBJECT
// ==========================================
extern OneButton button;

// ==========================================
// FUNCTION DECLARATIONS
// ==========================================

/**
 * Initialize button with callbacks
 */
void initButton();

/**
 * Callback for button single click
 */
void buttonClick();

/**
 * Callback for button long press
 */
void buttonLongPress();

/**
 * Update button state (call in loop)
 */
void updateButton();

#endif // BUTTON_H