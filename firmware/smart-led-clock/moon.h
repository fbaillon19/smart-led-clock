/**
 * @file moon.h
 * @brief Moon Phase Display Module - Header
 * 
 * Manages the lunar phase display system with stepper motor control,
 * optical sensor calibration, and astronomical calculations.
 * 
 * Features:
 * - Gaussian peak detection for precise home position
 * - 28BYJ-48 stepper motor control with overheat prevention
 * - LDR-based optical position sensing
 * - Monthly automatic recalibration
 * - 8-phase lunar cycle tracking
 * - Web interface integration
 * 
 * Hardware:
 * - 28BYJ-48 stepper motor + ULN2003 driver (D3, D4, D7, D8)
 * - LDR photoresistor (A1)
 * - Calibration LED (D12)
 * - 3D printed bicolor sphere with calibration disk
 * 
 * @author F. Baillon
 * @version 1.1.0
 * @date November 2025
 * @license MIT License
 */

#ifndef MOON_H
#define MOON_H

#include <Arduino.h>
#include <Stepper.h>
#include <math.h>
#include "config.h"
#include "string.h"
#include "display.h"
#include "button.h"
#include "rtc.h"


// ==========================================
// PIN DEFINITIONS
// ==========================================
#define PIN_MOON_STEPPER_IN1    3    ///< ULN2003 IN1 (stepper motor)
#define PIN_MOON_STEPPER_IN2    4    ///< ULN2003 IN2 (stepper motor)
#define PIN_MOON_STEPPER_IN3    7    ///< ULN2003 IN3 (stepper motor)
#define PIN_MOON_STEPPER_IN4    8    ///< ULN2003 IN4 (stepper motor)
#define PIN_MOON_LDR_SENSOR     A1   ///< LDR photoresistor (analog input)
#define PIN_MOON_CALIB_LED      12   ///< Calibration LED indicator

// ==========================================
// MOTOR CONFIGURATION
// ==========================================
#define MOON_STEPS_PER_REV      2048  ///< Steps per revolution (28BYJ-48 with 1:64 reduction)
#define MOON_SPEED_CALIB        5     ///< RPM for calibration scan
#define MOON_SPEED_NORMAL       10    ///< RPM for normal positioning
#define MOON_CALIB_STEP_SIZE    8     ///< Steps between LDR readings during calibration

// ==========================================
// SENSOR CONFIGURATION
// ==========================================
#define MOON_LDR_SAMPLE_COUNT   10    ///< Number of samples to average for LDR reading
#define MOON_LDR_SAMPLE_DELAY   10    ///< Milliseconds between LDR samples
#define MOON_PEAK_THRESHOLD     0.7   ///< Threshold for peak detection (70% of max)
#define MOON_MIN_PEAK_VALUE     300   ///< Minimum acceptable peak value for valid calibration

// ==========================================
// CALIBRATION SETTINGS
// ==========================================
#define MOON_CALIB_LED_BLINK    250   ///< LED blink rate during calibration (ms)
#define MOON_RECALIB_DAYS       30    ///< Days between automatic recalibrations

// ==========================================
// ASTRONOMICAL CONSTANTS
// ==========================================
#define MOON_LUNAR_CYCLE_DAYS   29.53058867  ///< Average lunar cycle duration in days (for incremental updates)
#define MOON_PHASES_COUNT       8             ///< Number of phases to track

// Julian Date constants for Meeus algorithm
#define JULIAN_EPOCH_OFFSET     2440587.5    ///< Unix epoch in Julian Days (Jan 1, 1970 00:00 UT)
#define SECONDS_PER_DAY         86400L       ///< Seconds in one day

// ==========================================
// UPDATE CONFIGURATION
// ==========================================
#define MOON_UPDATES_PER_DAY    3            ///< Position updates per day (3× = ~4° rotation each)
#define MOON_UPDATE_HOUR_1      2            ///< First update at 2:00 AM (night)
#define MOON_UPDATE_HOUR_2      10           ///< Second update at 10:00 AM (morning)
#define MOON_UPDATE_HOUR_3      18           ///< Third update at 18:00 (evening)
#define MOON_UPDATE_MINUTE      5            ///< Update at HH:05 to avoid conflicts

// ==========================================
// DATA STRUCTURES
// ==========================================

/**
 * @struct MoonPhaseData
 * @brief Current moon phase information
 */
struct MoonPhaseData {
  uint8_t phase;              ///< Current phase (0-7: New, Waxing Crescent, First Quarter, etc.)
  float exactPhase;           ///< Exact phase as float (0.0-8.0)
  float lunarAge;             ///< Days since last new moon (0.0-29.53)
  float illumination;         ///< Percentage of moon illuminated (0-100)
  int currentSteps;           ///< Current motor position in steps (0-2047)
  bool isCalibrated;          ///< Calibration status
  unsigned long lastUpdate;   ///< Timestamp of last position update (millis)
  unsigned long lastCalib;    ///< Timestamp of last calibration (epoch)
  
  // Meeus algorithm tracking (hybrid approach)
  unsigned long nextNewMoonEpoch;  ///< Next new moon timestamp (calculated by Meeus or incremented)
  unsigned long lastMeeusSync;     ///< Last Meeus algorithm synchronization (epoch)
  bool meeusInitialized;           ///< Meeus algorithm has been initialized
};

/**
 * @struct MoonCalibrationResult
 * @brief Result of calibration process
 */
struct MoonCalibrationResult {
  bool success;               ///< Calibration succeeded
  int peakStep;               ///< Step position of peak brightness
  int peakValue;              ///< LDR value at peak (0-1023)
  int finalValue;             ///< LDR value after positioning
  int difference;             ///< Difference between peak and final
  unsigned long duration;     ///< Calibration duration in milliseconds
};

// ==========================================
// GLOBAL VARIABLES (extern declarations)
// ==========================================
extern MoonPhaseData moonData;
extern MoonCalibrationResult lastCalibResult;

// ==========================================
// FUNCTION PROTOTYPES
// ==========================================

// Initialization
/**
 * @brief Initialize moon phase module
 * @return true if initialization successful, false otherwise
 */
bool initMoon();

// Calibration
/**
 * @brief Perform Gaussian peak calibration to find home position
 * @return MoonCalibrationResult structure with calibration details
 */
MoonCalibrationResult calibrateMoonHome();

/**
 * @brief Check if monthly recalibration is needed and perform if necessary
 * @param currentEpoch Current Unix timestamp
 * @return true if recalibration was performed
 */
bool checkAndRecalibrate(unsigned long currentEpoch);

// Position updates
/**
 * @brief Update moon position based on current date/time
 * @param currentEpoch Current Unix timestamp
 * @return true if update successful
 */
bool updateMoonPosition(unsigned long currentEpoch);

/**
 * @brief Move moon to specific phase position
 * @param phase Phase number (0-7)
 * @return true if movement successful
 */
bool moveMoonToPhase(uint8_t phase);

// Astronomical calculations
/**
 * @brief Calculate lunar age using hybrid Meeus + average cycle approach
 * @param currentEpoch Current Unix timestamp
 * @return Lunar age in days (0.0-29.53)
 */
float calculateLunarAge(unsigned long currentEpoch);

/**
 * @brief Calculate current moon phase
 * @param currentEpoch Current Unix timestamp
 * @return Phase number (0-7)
 */
uint8_t calculateMoonPhase(unsigned long currentEpoch);

/**
 * @brief Calculate exact moon phase as continuous value
 * @param currentEpoch Current Unix timestamp
 * @return Exact phase (0.0-8.0)
 */
float calculateExactMoonPhase(unsigned long currentEpoch);

/**
 * @brief Calculate moon illumination percentage
 * @param currentEpoch Current Unix timestamp
 * @return Illumination percentage (0-100)
 */
float calculateMoonIllumination(unsigned long currentEpoch);

/**
 * @brief Get moon phase name as string
 * @param phase Phase number (0-7)
 * @return Phase name (e.g., "New Moon", "Full Moon")
 */
const char* getMoonPhaseName(uint8_t phase);

// Meeus algorithm functions
/**
 * @brief Calculate next new moon using Jean Meeus algorithm
 * 
 * Implements the astronomical algorithm from "Astronomical Algorithms" 
 * by Jean Meeus (1998), Chapter 49. Provides precision of ±2 minutes.
 * 
 * This function should be called:
 * - At system initialization
 * - Every 6 lunar cycles (~6 months) for resynchronization
 * 
 * @param currentEpoch Current Unix timestamp
 * @return Unix timestamp of next new moon (UTC)
 */
unsigned long calculateNextNewMoonMeeus(unsigned long currentEpoch);

/**
 * @brief Calculate new moon Julian Date for given month
 * 
 * Core Meeus algorithm implementation. Calculates the Julian Date
 * of the new moon nearest to the specified month.
 * 
 * @param year Year (e.g., 2025)
 * @param month Month (1-12)
 * @return Julian Date of new moon (decimal days)
 */
double calculateMeeusNewMoon(int year, int month);

/**
 * @brief Check if new moon has passed and resync with Meeus
 * 
 * When current time exceeds nextNewMoonEpoch:
 * - Recalculates next new moon using Meeus algorithm (precise)
 * - Updates reference for next cycle
 * 
 * This ensures resynchronization at every new moon for maximum accuracy.
 * 
 * @param currentEpoch Current Unix timestamp
 * @return true if cycle was incremented and Meeus recalculation performed
 */
bool checkAndIncrementMoonCycle(unsigned long currentEpoch);

/**
 * @brief Check if it's time for scheduled moon update
 * 
 * Moon position updates occur 3 times per day at:
 * - 2:05 AM (night update - preferred)
 * - 10:05 AM (morning update)
 * - 18:05 (6:05 PM - evening update)
 * 
 * This provides ~4° rotation per update (instead of ~0.5° hourly).
 * Night updates avoid interfering with clock display operations.
 * 
 * @param currentEpoch Current Unix timestamp
 * @return true if current time matches one of the update schedules
 */
//bool isScheduledUpdateTime(unsigned long currentEpoch);

// Sensor functions
/**
 * @brief Read LDR sensor with averaging
 * @return Averaged analog value (0-1023)
 */
int readLDR();

// Motor control
/**
 * @brief Disable stepper motor coils to prevent overheating
 */
void disableMoonMotor();

/**
 * @brief Convert phase number to motor steps
 * @param phase Phase number (0-7)
 * @return Motor position in steps
 */
int phaseToSteps(uint8_t phase);

/**
 * @brief Convert exact phase to motor steps
 * @param exactPhase Exact phase (0.0-8.0)
 * @return Motor position in steps
 */
int exactPhaseToSteps(float exactPhase);

// Status and debugging
/**
 * @brief Get current moon phase data
 * @return Reference to MoonPhaseData structure
 */
MoonPhaseData& getMoonData();

/**
 * @brief Get last calibration result
 * @return Reference to MoonCalibrationResult structure
 */
MoonCalibrationResult& getLastCalibrationResult();

/**
 * @brief Get days since last calibration
 * @param currentEpoch Current Unix timestamp
 * @return Days since last calibration
 */
float daysSinceLastCalibration(unsigned long currentEpoch);

// Date/time conversion utilities (for Meeus algorithm)
/**
 * @brief Convert Unix epoch to date/time components
 * @param epoch Unix timestamp
 * @param year Output: year
 * @param month Output: month (1-12)
 * @param day Output: day (1-31)
 * @param hour Output: hour (0-23)
 * @param minute Output: minute (0-59)
 * @param second Output: second (0-59)
 */
void epochToDateTime(unsigned long epoch, int &year, int &month, int &day, int &hour, int &minute, int &second);

#endif // MOON_H