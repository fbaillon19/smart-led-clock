/**
 * @file leds.h
 * @brief LED rings and animations module
 * 
 * Manages all LED operations including analog clock display and animations.
 * 
 * LED strips:
 * - Hour ring: 12 NeoPixel LEDs (blue)
 * - Minute/Second ring: 60 NeoPixel LEDs (red/green)
 * - Air quality bar: 10 NeoPixel LEDs (color gradient)
 * 
 * Features:
 * - Analog clock display with hour/minute/second hands
 * - Overlap color for minute/second coincidence
 * - Hourly color animation
 * - Air quality visualization with dynamic color gradient
 * - Brightness control
 * 
 * @author F. Baillon
 * @version 1.1.0
 * @date November 2025
 * @license MIT License
 * 
 * Copyright (c) 2025 F. Baillon
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