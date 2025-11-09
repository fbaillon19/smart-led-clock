/**
 * @file leds.cpp
 * @brief LED rings and animations implementation
 * 
 * @author F. Baillon
 * @version 1.0.0
 * @date January 2025
 * @license MIT License
 * 
 * Copyright (c) 2025 F. Baillon
 */

#include "leds.h"
#include "display.h"
#include "rtc.h"
#include <Arduino.h>

// ==========================================
// GLOBAL LED OBJECTS
// ==========================================
Adafruit_NeoPixel ledsHour(NUM_LEDS_HOUR, PIN_LEDS_HOUR, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledsMinuteSec(NUM_LEDS_MINUTE_SECOND, PIN_LEDS_MINUTE_SECOND, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledsAirQuality(NUM_LEDS_AIR_QUALITY, PIN_LEDS_AIR_QUALITY, NEO_GRB + NEO_KHZ800);

// ==========================================
// FUNCTION IMPLEMENTATIONS
// ==========================================

/**
 * @brief Initialize all LED strips
 * 
 * Configures and clears all NeoPixel LED strips:
 * - Hour ring (12 LEDs)
 * - Minute/Second ring (60 LEDs)
 * - Air quality bar (10 LEDs)
 * 
 * Sets initial brightness to 100 (out of 255) for comfortable viewing.
 * All LEDs are cleared (turned off) after initialization.
 */
void initLEDs() {
  ledsHour.begin();
  ledsMinuteSec.begin();
  ledsAirQuality.begin();
  
  ledsHour.setBrightness(100);
  ledsMinuteSec.setBrightness(100);
  ledsAirQuality.setBrightness(100);
  
  ledsHour.clear();
  ledsMinuteSec.clear();
  ledsAirQuality.clear();
  
  ledsHour.show();
  ledsMinuteSec.show();
  ledsAirQuality.show();
  
  DEBUG_PRINTLN("LED strips initialized");
}

/**
 * @brief Update LED clock display
 * 
 * Updates the analog clock display on LED rings:
 * - Hour ring: Shows current hour (12-hour format)
 * - Minute/Second ring: Shows both minute and second
 * 
 * Special handling:
 * - When minute and second coincide: Shows overlap color (yellow)
 * - Previous positions are cleared to black before updating
 * - Only updates changed LEDs for efficiency
 * 
 * Colors (defined in config.h):
 * - Hour: Blue
 * - Minute: Red
 * - Second: Green
 * - Overlap: Yellow
 * 
 * @param now Current DateTime from RTC
 */
void updateLEDClock(DateTime now) {
  int hour = now.hour() % 12;
  int minute = now.minute();
  int second = now.second();

  // Update hour LED
  if (hour != lastHour) {
    ledsHour.setPixelColor(lastHour, 0, 0, 0);
    lastHour = hour;
  }
  ledsHour.setPixelColor(hour, runtimeColorHourR, runtimeColorHourG, runtimeColorHourB);
  ledsHour.show();

  // Update minute LED
  if (minute != lastMinute) {
    ledsMinuteSec.setPixelColor(lastMinute, 0, 0, 0);
    lastMinute = minute;
  }

  // Clear old second LED (if not overlapping with minute)
  if (lastSecond != 61 && lastSecond != minute) {
    ledsMinuteSec.setPixelColor(lastSecond, 0, 0, 0);
  }

  // Set minute and second LEDs (handle overlap)
  if (minute == second) {
    // Overlap: show combined color (garder COLOR_OVERLAP pour l'instant)
    ledsMinuteSec.setPixelColor(minute, COLOR_OVERLAP_R, COLOR_OVERLAP_G, COLOR_OVERLAP_B);
  } else {
    ledsMinuteSec.setPixelColor(minute, runtimeColorMinuteR, runtimeColorMinuteG, runtimeColorMinuteB);
    ledsMinuteSec.setPixelColor(second, runtimeColorSecondR, runtimeColorSecondG, runtimeColorSecondB);
  }

  ledsMinuteSec.show();
  lastSecond = second;
}

/**
 * @brief Update air quality LED bar
 * 
 * Displays air quality as a color gradient on 10-LED bar.
 * Color shifts based on AQI (Air Quality Index):
 * 
 * Color mapping (HSV color wheel):
 * - AQI 0-50 (Good): Cyan-green to green
 * - AQI 51-100 (Moderate): Green to yellow-green
 * - AQI 101-200 (Unhealthy): Yellow to orange
 * - AQI 201-500 (Very unhealthy/Hazardous): Orange to red
 * 
 * Brightness increases slightly with worse air quality (20-60).
 * Each LED in the bar has a subtle hue offset to create a gradient effect.
 * 
 * Uses global airQuality.estimatedAQI from sensors module.
 */
void updateAirQualityLEDs() {
  // Calculate brightness based on AQI (subtle increase for worse air)
  int brightness = constrain(20 + (airQuality.estimatedAQI / 10), 20, 60);
  ledsAirQuality.setBrightness(brightness);
  ledsAirQuality.clear();
  
  // Calculate base hue color based on AQI level
  int baseHue;
  if (airQuality.estimatedAQI <= 50) {
    // Good: Cyan-green to green
    baseHue = map(airQuality.estimatedAQI, 0, 50, 26000, 21845);
  } else if (airQuality.estimatedAQI <= 100) {
    // Moderate: Green to yellow-green
    baseHue = map(airQuality.estimatedAQI, 50, 100, 21845, 16384);
  } else if (airQuality.estimatedAQI <= 200) {
    // Unhealthy: Yellow to orange
    baseHue = map(airQuality.estimatedAQI, 100, 200, 16384, 4096);
  } else {
    // Very unhealthy/Hazardous: Orange to red
    baseHue = map(airQuality.estimatedAQI, 200, 500, 4096, 0);
  }
  
  // Fill all 10 LEDs with gradient
  for (int i = 0; i < NUM_LEDS_AIR_QUALITY; i++) {
    // Create subtle gradient across the bar
    // Bottom LEDs slightly cooler, top LEDs slightly warmer
    int hueOffset = (i - NUM_LEDS_AIR_QUALITY / 2) * 500;
    int ledHue = baseHue + hueOffset;
    
    // Keep hue in valid range (0-65535)
    if (ledHue < 0) ledHue += 65536;
    if (ledHue >= 65536) ledHue -= 65536;
    
    uint32_t color = ledsAirQuality.gamma32(ledsAirQuality.ColorHSV(ledHue));
    ledsAirQuality.setPixelColor(i, color);
  }
  
  ledsAirQuality.show();
}

/**
 * @brief Start hourly animation
 * 
 * Triggered every hour on the hour (when minutes and seconds are both 0).
 * 
 * Animation behavior:
 * - Displays rainbow effect on minute/second ring
 * - Lasts approximately 5 seconds (100 steps × 50ms)
 * - Clears minute/second ring at start
 * - Shows animation message on LCD (if backlight is on)
 * 
 * Sets isAnimationActive flag to pause normal clock updates.
 */
void startAnimation() {
  DEBUG_PRINTLN("Starting hourly animation");
  
  if (lcdBacklightOn) {
    showAnimationMessage();
  }
  
  isAnimationActive = true;
  animationStep = 0;
  animationHue = 0;
  ledsMinuteSec.clear();
  ledsMinuteSec.show();
}

/**
 * @brief Update animation frame
 * 
 * Called every 50ms during hourly animation.
 * Creates a rainbow chase effect:
 * - Lights every 3rd LED in a rotating pattern
 * - Colors cycle through full HSV rainbow spectrum
 * - Hue advances 1/100th of spectrum per frame
 * 
 * Animation runs for 100 frames (5 seconds total).
 * 
 * @return true if animation continues, false if complete (100 frames done)
 */
bool updateAnimation() {
  if (animationStep >= 100) {
    return false;
  }

  ledsMinuteSec.clear();

  // Light every 3rd LED in rotating pattern
  for (int i = animationStep % 3; i < NUM_LEDS_MINUTE_SECOND; i += 3) {
    int hue = animationHue + (i * 65536L / NUM_LEDS_MINUTE_SECOND);
    uint32_t color = ledsMinuteSec.gamma32(ledsMinuteSec.ColorHSV(hue));
    ledsMinuteSec.setPixelColor(i, color);
  }

  ledsMinuteSec.show();
  animationHue += 65536 / 100;  // Advance hue 1/100th per frame
  animationStep++;
  
  return true;
}

/**
 * @brief Stop animation and restore clock
 * 
 * Called when hourly animation completes (after 100 frames).
 * 
 * Cleanup process:
 * 1. Clear isAnimationActive flag
 * 2. Clear minute/second ring LEDs
 * 3. Clear LCD display
 * 4. Reset last LED tracking variables
 * 5. Restore normal clock display
 * 6. Update LCD with current data (if backlight is on)
 * 
 * Forces all LEDs to update by resetting tracking variables to invalid values.
 */
void stopAnimation() {
  DEBUG_PRINTLN("Animation complete");
  isAnimationActive = false;
  ledsMinuteSec.clear();
  ledsMinuteSec.show();
  
  clearLCD();
  
  DateTime now = getCurrentTime();
  
  // Reset tracking to force full update
  lastSecond = 61;
  lastMinute = 61;
  lastHour = 25;
  
  updateLEDClock(now);
  
  if (lcdBacklightOn) {
    updateLCDDisplay(now);
  }
}