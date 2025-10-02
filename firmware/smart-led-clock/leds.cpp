/**
 * Smart LED Clock - LEDs Module Implementation
 * 
 * Author: F. Baillon
 * Version: Phase 5
 * Date: January 2025
 * License: GPL v3.0
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
  
  Serial.println("LED strips initialized");
}

void updateLEDClock(DateTime now) {
  int hour = now.hour() % 12;
  int minute = now.minute();
  int second = now.second();

  // Update hour LED
  if (hour != lastHour) {
    ledsHour.setPixelColor(lastHour, 0, 0, 0);
    lastHour = hour;
  }
  ledsHour.setPixelColor(hour, COLOR_HOUR_R, COLOR_HOUR_G, COLOR_HOUR_B);
  ledsHour.show();

  // Update minute LED
  if (minute != lastMinute) {
    ledsMinuteSec.setPixelColor(lastMinute, 0, 0, 0);
    lastMinute = minute;
  }

  // Clear old second LED
  if (lastSecond != 61 && lastSecond != minute) {
    ledsMinuteSec.setPixelColor(lastSecond, 0, 0, 0);
  }

  // Set minute and second LEDs
  if (minute == second) {
    // Overlap: show combined color
    ledsMinuteSec.setPixelColor(minute, COLOR_OVERLAP_R, COLOR_OVERLAP_G, COLOR_OVERLAP_B);
  } else {
    ledsMinuteSec.setPixelColor(minute, COLOR_MINUTE_R, COLOR_MINUTE_G, COLOR_MINUTE_B);
    ledsMinuteSec.setPixelColor(second, COLOR_SECOND_R, COLOR_SECOND_G, COLOR_SECOND_B);
  }

  ledsMinuteSec.show();
  lastSecond = second;
}

void updateAirQualityLEDs() {
  int brightness = constrain(20 + (airQuality.estimatedAQI / 10), 20, 60);
  ledsAirQuality.setBrightness(brightness);
  ledsAirQuality.clear();
  
  // Calculate base color based on AQI
  int baseHue;
  if (airQuality.estimatedAQI <= 50) {
    baseHue = map(airQuality.estimatedAQI, 0, 50, 26000, 21845);
  } else if (airQuality.estimatedAQI <= 100) {
    baseHue = map(airQuality.estimatedAQI, 50, 100, 21845, 16384);
  } else if (airQuality.estimatedAQI <= 200) {
    baseHue = map(airQuality.estimatedAQI, 100, 200, 16384, 4096);
  } else {
    baseHue = map(airQuality.estimatedAQI, 200, 500, 4096, 0);
  }
  
  // Fill all LEDs with gradient
  for (int i = 0; i < NUM_LEDS_AIR_QUALITY; i++) {
    int hueOffset = (i - NUM_LEDS_AIR_QUALITY / 2) * 500;
    int ledHue = baseHue + hueOffset;
    
    // Keep hue in valid range
    if (ledHue < 0) ledHue += 65536;
    if (ledHue >= 65536) ledHue -= 65536;
    
    uint32_t color = ledsAirQuality.gamma32(ledsAirQuality.ColorHSV(ledHue));
    ledsAirQuality.setPixelColor(i, color);
  }
  
  ledsAirQuality.show();
}

void startAnimation() {
  Serial.println("Starting hourly animation");
  
  if (lcdBacklightOn) {
    showAnimationMessage();
  }
  
  isAnimationActive = true;
  animationStep = 0;
  animationHue = 0;
  ledsMinuteSec.clear();
  ledsMinuteSec.show();
}

bool updateAnimation() {
  if (animationStep >= 100) {
    return false;
  }

  ledsMinuteSec.clear();

  for (int i = animationStep % 3; i < NUM_LEDS_MINUTE_SECOND; i += 3) {
    int hue = animationHue + (i * 65536L / NUM_LEDS_MINUTE_SECOND);
    uint32_t color = ledsMinuteSec.gamma32(ledsMinuteSec.ColorHSV(hue));
    ledsMinuteSec.setPixelColor(i, color);
  }

  ledsMinuteSec.show();
  animationHue += 65536 / 100;
  animationStep++;
  
  return true;
}

void stopAnimation() {
  Serial.println("Animation complete");
  isAnimationActive = false;
  ledsMinuteSec.clear();
  ledsMinuteSec.show();
  
  clearLCD();
  
  DateTime now = getCurrentTime();
  lastSecond = 61;
  lastMinute = 61;
  lastHour = 25;
  updateLEDClock(now);
  
  if (lcdBacklightOn) {
    updateLCDDisplay(now);
  }
}