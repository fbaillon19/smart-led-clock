/**
 * Moon Phase Calculations
 * 
 * Pure mathematical functions for calculating lunar phases and positions.
 * No hardware dependencies - can be used in testing and production code.
 * 
 * Based on:
 * - Meeus, Jean. "Astronomical Algorithms" (1991)
 * - Simplified algorithms for embedded systems
 * 
 * Precision: ±0.5 days over decades (sufficient for visual display)
 * 
 * Author: F. Baillon
 * Version: 1.0.0
 * License: MIT
 */

#ifndef MOON_CALCULATIONS_H
#define MOON_CALCULATIONS_H

#include <Arduino.h>
#include <math.h>

// ==========================================
// CONSTANTS
// ==========================================

// Known new moon reference (Oct 21, 2025, 12:25 UTC)
// This is used as epoch for all calculations
#define MOON_KNOWN_NEW_MOON     1761049500L  // Unix timestamp


// Average lunar cycle duration in days
#define MOON_LUNAR_CYCLE_DAYS 29.53058867

// Steps per revolution for 28BYJ-48 stepper motor with gear reduction
#define MOON_STEPS_PER_REVOLUTION 2048

// ==========================================
// PHASE NAMES
// ==========================================

/**
 * Get the name of a lunar phase
 * @param phase Phase number (0-7)
 * @return Phase name as String
 */
String getMoonPhaseName(uint8_t phase) {
  const char* phaseNames[8] = {
    "Nouvelle lune",           // 🌑 Phase 0
    "Premier croissant",      // 🌒 Phase 1
    "Premier quartier",       // 🌓 Phase 2
    "Gibbeuse croissante",    // 🌔 Phase 3
    "Pleine lune",            // 🌕 Phase 4
    "Gibbeuse décroissante",  // 🌖 Phase 5
    "Dernier quartier",       // 🌗 Phase 6
    "Dernier croissant"       // 🌘 Phase 7
  };
  
  return String(phaseNames[phase % 8]);
}

// ==========================================
// LUNAR AGE CALCULATIONS
// ==========================================

/**
 * Calculate the age of the moon in days since last new moon
 * @param currentEpoch Current Unix timestamp
 * @return Lunar age in days (0.0 to 29.53)
 */
float getLunarAge(unsigned long currentEpoch) {
  // Calculate days elapsed since known new moon
  long secondsSinceNew = currentEpoch - MOON_KNOWN_NEW_MOON;
  float daysSinceNew = secondsSinceNew / 86400.0;
  
  // Calculate position in current cycle using modulo
  float lunarAge = fmod(daysSinceNew, MOON_LUNAR_CYCLE_DAYS);
  
  // Handle negative values (shouldn't happen with future dates)
  if (lunarAge < 0)   lunarAge += MOON_LUNAR_CYCLE_DAYS;
  
  return lunarAge;
}

// ==========================================
// PHASE CALCULATIONS
// ==========================================
/**
 * Calculate the current moon phase (discrete 0-7)
 * @param currentEpoch Current Unix timestamp
 * @return Phase number (0-7)
 */
uint8_t calculateMoonPhase(unsigned long currentEpoch) {
  float lunarAge = getLunarAge(currentEpoch);
  
  // Convert lunar age to phase (0-7)
  if (lunarAge < (MOON_LUNAR_CYCLE_DAYS/16.0))        return 0;     // New Moon
  if (lunarAge < (3 * MOON_LUNAR_CYCLE_DAYS/16.0))    return 1;     // Waxing Crescent
  if (lunarAge < (5 * MOON_LUNAR_CYCLE_DAYS/16.0))    return 2;     // First Quarter
  if (lunarAge < (7 * MOON_LUNAR_CYCLE_DAYS/16.0))    return 3;     // Waxing Gibbous
  if (lunarAge < (9 * MOON_LUNAR_CYCLE_DAYS/16.0))    return 4;     // Full Moon
  if (lunarAge < (11 * MOON_LUNAR_CYCLE_DAYS/16.0))   return 5;     // Waning Gibbous
  if (lunarAge < (13 * MOON_LUNAR_CYCLE_DAYS/16.0))   return 6;     // Last Quarter
  if (lunarAge < (15 * MOON_LUNAR_CYCLE_DAYS/16.0))   return 7;     // Waning Gibbous
  if (lunarAge < MOON_LUNAR_CYCLE_DAYS)               return 0;     // New Moon
}

/**
 * Calculate moon illumination percentage
 * @param currentEpoch Current Unix timestamp
 * @return Illumination percentage (0-100)
 */
float getMoonIllumination(unsigned long currentEpoch) {
  float lunarAge = getLunarAge(currentEpoch);
  
  // Illumination follows a cosine curve
  // 0 days = 0%, 14.76 days = 100%, 29.53 days = 0%
  float illumination = 50.0 * (1.0 - cos((lunarAge / MOON_LUNAR_CYCLE_DAYS) * 2.0 * PI));
  
  return illumination;
}

// ==========================================
// MOTOR POSITION CALCULATIONS
// ==========================================

/**
 * Convert moon phase to stepper motor steps
 * @param phase Phase number (0-7)
 * @return Number of steps from home position
 */
int phaseToSteps(uint8_t phase) {
  // Each phase = 2048 steps / 8 phases = 256 steps
  return (phase * MOON_STEPS_PER_REVOLUTION) / 8;
}

/**
 * Convert stepper steps to phase number
 * @param steps Current step position
 * @return Phase number (0-7)
 */
uint8_t stepsToPhase(int steps) {
  // Normalize steps to 0-2047 range
  steps = steps % MOON_STEPS_PER_REVOLUTION;
  if (steps < 0) steps += MOON_STEPS_PER_REVOLUTION;
  
  // Convert to phase
  return (uint8_t)((steps * 8) / MOON_STEPS_PER_REVOLUTION);
}

/**
 * Calculate shortest path between two positions (handles wrap-around)
 * @param currentSteps Current position
 * @param targetSteps Target position
 * @return Steps to move (negative = counterclockwise)
 */
int calculateShortestPath(int currentSteps, int targetSteps) {
  int diff = targetSteps - currentSteps;
  
  // Handle wrap-around
  if (diff > MOON_STEPS_PER_REVOLUTION / 2) {
    diff -= MOON_STEPS_PER_REVOLUTION;
  } else if (diff < -MOON_STEPS_PER_REVOLUTION / 2) {
    diff += MOON_STEPS_PER_REVOLUTION;
  }
  
  return diff;
}

#endif MOON_CALCULATIONS_H