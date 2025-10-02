/**
 * Smart LED Clock - Button Module
 * 
 * Handles button input with OneButton library:
 * - Single click: Wake LCD or cycle display modes
 * - Long press: Return to default mode
 * 
 * Author: F. Baillon
 * Version: Phase 5
 * Date: January 2025
 * License: GPL v3.0
 */

#ifndef BUTTON_H
#define BUTTON_H

#include <OneButton.h>
#include <LiquidCrystal_I2C.h>
#include "config.h"

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