/**
 * Smart LED Clock - LEDs Module
 * 
 * Handles all LED operations:
 * - Hour ring (12 LEDs)
 * - Minute/Second ring (60 LEDs)
 * - Air quality bar (10 LEDs)
 * - Hourly animations
 * 
 * Author: F. Baillon
 * Version: Phase 5
 * Date: January 2025
 * License: GPL v3.0
 */

#ifndef LEDS_H
#define LEDS_H

#include <Adafruit_NeoPixel.h>
#include <RTClib.h>
#include "config.h"

// ==========================================
// LED OBJECTS
// ==========================================
extern Adafruit_NeoPixel ledsHour;
extern Adafruit_NeoPixel ledsMinuteSec;
extern Adafruit_NeoPixel ledsAirQuality;

// ==========================================
// FUNCTION DECLARATIONS
// ==========================================

/**
 * Initialize all LED strips
 */
void initLEDs();

/**
 * Update LED clock display
 * @param now Current DateTime
 */
void updateLEDClock(DateTime now);

/**
 * Update air quality LED bar
 */
void updateAirQualityLEDs();

/**
 * Start hourly animation
 */
void startAnimation();

/**
 * Update animation frame
 * @return true if animation continues, false if complete
 */
bool updateAnimation();

/**
 * Stop animation and restore clock
 */
void stopAnimation();

#endif // LEDS_H