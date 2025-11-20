/**
 * @file moon.cpp
 * @brief Moon Phase Display Module - Implementation
 * 
 * @author F. Baillon
 * @version 1.1.0
 * @date November 2025
 * @license MIT License
 */

#include "moon.h"


// ==========================================
// GLOBAL VARIABLES (definitions)
// ==========================================
MoonPhaseData moonData = {
  0,      // phase
  0.0,    // exactPhase
  0.0,    // lunarAge
  0.0,    // illumination
  0,      // currentSteps
  false,  // isCalibrated
  0,      // lastUpdate
  0,      // lastCalib
  0,      // nextNewMoonEpoch
  0,      // lastMeeusSync
  false   // meeusInitialized
};
MoonCalibrationResult lastCalibResult = {false, 0, 0, 0, 0, 0};

// Stepper motor instance
static Stepper moonStepper(MOON_STEPS_PER_REV, 
                           PIN_MOON_STEPPER_IN1, 
                           PIN_MOON_STEPPER_IN3,  // Note: IN3 before IN2 for correct sequence
                           PIN_MOON_STEPPER_IN2, 
                           PIN_MOON_STEPPER_IN4);

// ==========================================
// PHASE NAMES
// ==========================================
static const char* MOON_PHASE_NAMES[8] = {
  MOON_PHASE_NEW_MOON,
  MOON_PHASE_WAXING_CRESCENT,
  MOON_PHASE_FIRST_QUARTER,
  MOON_PHASE_WAXING_GIBBUS,
  MOON_PHASE_FULL_MOON,
  MOON_PHASE_WANING_GIBBUS,
  MOON_PHASE_LAST_QUARTER,
  MOON_PHASE_WANING_CRESCENT
};

// ==========================================
// PRIVATE HELPER FUNCTIONS
// ==========================================

/**
 * @brief Blink calibration LED
 * @param times Number of blinks
 * @param delayMs Delay between blinks
 */
static void blinkCalibrationLED(int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(PIN_MOON_CALIB_LED, HIGH);
    delay(delayMs);
    digitalWrite(PIN_MOON_CALIB_LED, LOW);
    delay(delayMs);
  }
}

// ==========================================
// INITIALIZATION
// ==========================================

/**
 * @brief Initialize moon phase module
 * 
 * Complete initialization sequence:
 * 1. Configure hardware (LDR, LED, motor)
 * 2. Test LDR sensor
 * 3. Perform Gaussian calibration to find home position (new moon reference)
 * 4. Display instructions for manual sphere orientation
 * 5. Wait for user button press confirmation
 * 6. Initialize Meeus algorithm and calculate next new moon
 * 7. Move sphere to current moon phase position
 * 
 * @return true if initialization successful, false otherwise
 */
bool initMoon() {
  DEBUG_PRINTLN("[MOON] Initializing moon phase module...");
  
  // Configure pins
  pinMode(PIN_MOON_LDR_SENSOR, INPUT);
  pinMode(PIN_MOON_CALIB_LED, OUTPUT);
  digitalWrite(PIN_MOON_CALIB_LED, LOW);
  
  // Set motor speed
  moonStepper.setSpeed(MOON_SPEED_NORMAL);
  
  // Initialize moon data structure
  moonData.phase = 0;
  moonData.exactPhase = 0.0;
  moonData.lunarAge = 0.0;
  moonData.illumination = 0.0;
  moonData.currentSteps = 0;
  moonData.isCalibrated = false;
  moonData.lastUpdate = 0;
  moonData.lastCalib = 0;
  moonData.nextNewMoonEpoch = 0;
  moonData.lastMeeusSync = 0;
  moonData.meeusInitialized = false;
  
  DEBUG_PRINTLN("[MOON] Hardware initialized");
  
  // Test LDR sensor
  int ldrTest = readLDR();
  DEBUG_PRINT("[MOON] LDR test reading: ");
  DEBUG_PRINT(ldrTest);
  DEBUG_PRINTLN(" / 1023");
  
  if (ldrTest == 0 || ldrTest == 1023) {
    DEBUG_PRINTLN("[MOON] WARNING: LDR reading at extreme value");
    DEBUG_PRINTLN("[MOON] Check sensor wiring before calibration");
  }
  
  // Perform initial Gaussian calibration
  DEBUG_PRINTLN("[MOON] Starting Gaussian calibration...");
  DEBUG_PRINTLN("[MOON] This will take approximately 40 seconds");
  displayStartupMessage(STR_MOON_CALIBRATION);
  
  lastCalibResult = calibrateMoonHome();
  
  if (!lastCalibResult.success) {
    DEBUG_PRINTLN("[MOON] ✗ Calibration failed");
    DEBUG_PRINTLN("[MOON] Moon phase display will be unavailable");
    displayStartupMessage(STR_MOON_CALIBR_ERROR);
    delay(2000);
    return false;
  }
  
  DEBUG_PRINTLN("[MOON] ✓ Calibration successful");
  DEBUG_PRINT("[MOON] Peak value: ");
  DEBUG_PRINT(lastCalibResult.peakValue);
  DEBUG_PRINTLN(" / 1023");
  
  // === MANUAL ORIENTATION SEQUENCE ===
  DEBUG_PRINTLN("[MOON] === Manual Orientation Required ===");
  DEBUG_PRINTLN("[MOON] Please orient the sphere manually:");
  DEBUG_PRINTLN("[MOON] - Black face must be fully visible");
  DEBUG_PRINTLN("[MOON] - Press button when ready");
  
  // Display moon calibration instructions on LCD
  displayMoonCalibInstructions();
  
  // Wait for button click using OneButton library
  // Reset any previous state
  button.reset();
  
  // Wait for a single click
  bool clicked = false;
  unsigned long startWait = millis();
  
  while (!clicked && (millis() - startWait < 300000)) {  // Timeout 5 minutes
    updateButton();  // Process button events
    
    // Check if single click occurred (not long press, not double click)
    if (button.getNumberClicks() == 1) {
      clicked = true;
    }
    
    delay(10);
  }
  button.reset();     // Reset current state
  
  if (!clicked) {
    DEBUG_PRINTLN("[MOON] ✗ Button timeout - continuing anyway");
  } else {
    DEBUG_PRINTLN("[MOON] ✓ User confirmed manual orientation");
  }
  
  displayStartupMessage(STR_MOON_ORIENT_OK);
  delay(1000);
  
  // === MEEUS ALGORITHM INITIALIZATION ===
  DEBUG_PRINTLN("[MOON] Initializing Meeus algorithm...");
  displayStartupMessage(STR_MOON_COMPUT_PHASE);
  
  // Get current time from RTC
  DateTime now = getCurrentTime();
  unsigned long currentEpoch = now.unixtime();
  
  // Calculate next new moon using Meeus algorithm
  moonData.nextNewMoonEpoch = calculateNextNewMoonMeeus(currentEpoch);
  moonData.lastMeeusSync = currentEpoch;
  moonData.meeusInitialized = true;
  
  DEBUG_PRINT("[MOON] Next new moon epoch: ");
  DEBUG_PRINTLN(moonData.nextNewMoonEpoch);
  
  // Calculate current phase and move to position
  DEBUG_PRINTLN("[MOON] Moving to current moon phase...");
  displayStartupMessage(STR_MOON_POSITION);
  
  if (updateMoonPosition(currentEpoch)) {
    DEBUG_PRINTLN("[MOON] ✓✓✓ INITIALIZATION COMPLETE ✓✓✓");
    DEBUG_PRINT("[MOON] Current phase: ");
    DEBUG_PRINT(moonData.phase);
    DEBUG_PRINT(" - ");
    DEBUG_PRINTLN(getMoonPhaseName(moonData.phase));
    displayStartupMessage(STR_MOON_POSITIONED);
    delay(2000);
    return true;
  } else {
    DEBUG_PRINTLN("[MOON] ✗ Failed to position moon");
    displayStartupMessage(STR_MOON_POSI_ERROR);
    delay(2000);
    return false;
  }
}

// ==========================================
// CALIBRATION
// ==========================================

MoonCalibrationResult calibrateMoonHome() {
  MoonCalibrationResult result = {false, 0, 0, 0, 0, 0};
  unsigned long startTime = millis();
  
  DEBUG_PRINTLN("[MOON] === Gaussian Peak Calibration ===");
  DEBUG_PRINTLN("[MOON] Scanning for brightness peak...");
  
  moonStepper.setSpeed(MOON_SPEED_CALIB);
  
  // Storage for peak detection
  int peakStep = 0;
  int peakValue = 0;
  int currentStep = 0;
  
  // LED blink state
  unsigned long lastBlink = 0;
  bool ledState = false;
  
  // Turn on LED to indicate calibration started
  digitalWrite(PIN_MOON_CALIB_LED, HIGH);
  
  // Scan full revolution
  for (currentStep = 0; currentStep < MOON_STEPS_PER_REV; currentStep += MOON_CALIB_STEP_SIZE) {
    // Blink LED during search
    if (millis() - lastBlink > MOON_CALIB_LED_BLINK) {
      ledState = !ledState;
      digitalWrite(PIN_MOON_CALIB_LED, ledState);
      lastBlink = millis();
    }
    
    // Move motor
    moonStepper.step(MOON_CALIB_STEP_SIZE);
    delay(20);  // Stabilization delay
    
    // Read LDR
    int ldrValue = readLDR();
    
    // Update peak if new maximum found
    if (ldrValue > peakValue) {
      peakValue = ldrValue;
      peakStep = currentStep;
    }
  }
  
  // Celebrate with LED blinks but keep LED ON for verification
  digitalWrite(PIN_MOON_CALIB_LED, HIGH);
  delay(200);
  digitalWrite(PIN_MOON_CALIB_LED, LOW);
  delay(200);
  digitalWrite(PIN_MOON_CALIB_LED, HIGH);
  delay(200);
  digitalWrite(PIN_MOON_CALIB_LED, LOW);
  delay(200);
  digitalWrite(PIN_MOON_CALIB_LED, HIGH);
  delay(200);
  digitalWrite(PIN_MOON_CALIB_LED, LOW);
  delay(200);
  
  // Keep LED ON for positioning and verification
  digitalWrite(PIN_MOON_CALIB_LED, HIGH);
  
  DEBUG_PRINTLN("[MOON] Scan complete!");
  DEBUG_PRINT("[MOON] Peak detected at step: ");
  DEBUG_PRINTLN(peakStep);
  DEBUG_PRINT("[MOON] Peak brightness value: ");
  DEBUG_PRINT(peakValue);
  DEBUG_PRINTLN(" / 1023");
  
  // Validate peak quality
  if (peakValue < MOON_MIN_PEAK_VALUE) {
    DEBUG_PRINTLN("[MOON] ✗ ERROR: Peak value too low (< " + String(MOON_MIN_PEAK_VALUE) + ")");
    DEBUG_PRINTLN("[MOON] Check: hole size, LDR positioning, ambient light");
    digitalWrite(PIN_MOON_CALIB_LED, LOW);
    disableMoonMotor();
    result.success = false;
    result.peakValue = peakValue;
    result.duration = millis() - startTime;
    return result;
  }
  
  DEBUG_PRINTLN("[MOON] Peak quality is good");
  
  // Calculate forward movement back to peak (unidirectional rotation only)
  int stepsToMove = (MOON_STEPS_PER_REV + peakStep - (MOON_STEPS_PER_REV % MOON_CALIB_STEP_SIZE)) % MOON_STEPS_PER_REV;
  
  DEBUG_PRINTLN("[MOON] Returning to peak position...");
  DEBUG_PRINT("[MOON] Forward movement: ");
  DEBUG_PRINT(stepsToMove);
  DEBUG_PRINTLN(" steps");
  
  moonStepper.step(stepsToMove);
  
  // Disable motor to prevent heating
  disableMoonMotor();
  
  // Set this as our home position (New Moon = phase 0)
  moonData.currentSteps = 0;
  moonData.isCalibrated = true;
  moonData.lastCalib = millis() / 1000;  // Store as seconds since boot (will be updated with epoch later)
  
  DEBUG_PRINTLN("[MOON] Positioned at peak (home position)");
  
  // Verify final position BEFORE turning off LED
  delay(200);  // Let motor settle
  int finalValue = readLDR();
  
  DEBUG_PRINT("[MOON] Final LDR reading: ");
  DEBUG_PRINT(finalValue);
  DEBUG_PRINTLN(" / 1023");
  
  int difference = abs(finalValue - peakValue);
  
  if (difference < 50) {
    DEBUG_PRINTLN("[MOON] ✓ Position verified - at maximum brightness");
    result.success = true;
  } else {
    DEBUG_PRINTLN("[MOON] ⚠ Position verification warning");
    DEBUG_PRINT("[MOON] Expected: ");
    DEBUG_PRINT(peakValue);
    DEBUG_PRINT(", Got: ");
    DEBUG_PRINTLN(finalValue);
    DEBUG_PRINT("[MOON] Difference: ");
    DEBUG_PRINTLN(difference);
    
    if (difference > 200) {
      DEBUG_PRINTLN("[MOON] ⚠ Large difference - possible motor step loss");
      DEBUG_PRINTLN("[MOON] Consider: reducing speed, checking friction");
    }
    
    // Still mark as success if peak was good, just position might be slightly off
    result.success = (peakValue >= MOON_MIN_PEAK_VALUE);
  }
  
  DEBUG_PRINTLN("[MOON] ✓✓✓ CALIBRATION COMPLETE ✓✓✓");
  DEBUG_PRINTLN("[MOON] System calibrated. Position = 0 (New Moon reference)");
  
  // Turn off LED AFTER verification
  digitalWrite(PIN_MOON_CALIB_LED, LOW);
  
  // Fill result structure
  result.peakStep = peakStep;
  result.peakValue = peakValue;
  result.finalValue = finalValue;
  result.difference = difference;
  result.duration = millis() - startTime;
  
  return result;
}

bool checkAndRecalibrate(unsigned long currentEpoch) {
  if (!moonData.isCalibrated) {
    return false;  // Can't check if never calibrated
  }
  
  float daysSinceCalib = daysSinceLastCalibration(currentEpoch);
  
  if (daysSinceCalib >= MOON_RECALIB_DAYS) {
    DEBUG_PRINTLN("[MOON] Monthly recalibration due");
    DEBUG_PRINT("[MOON] Days since last calibration: ");
    DEBUG_PRINTLN(daysSinceCalib);
    
    MoonCalibrationResult result = calibrateMoonHome();
    
    if (result.success) {
      moonData.lastCalib = currentEpoch;
      DEBUG_PRINTLN("[MOON] ✓ Recalibration successful");
      return true;
    } else {
      DEBUG_PRINTLN("[MOON] ✗ Recalibration failed");
      return false;
    }
  }
  
  return false;  // No recalibration needed
}

// ==========================================
// POSITION UPDATES
// ==========================================

/**
 * @brief Update moon position based on current date/time
 * 
 * Checks if moon cycle needs to be incremented, then calculates
 * current phase and moves motor if needed. Uses unidirectional
 * rotation only (always moves forward).
 * 
 * @param currentEpoch Current Unix timestamp
 * @return true if update successful
 */
bool updateMoonPosition(unsigned long currentEpoch) {
  if (!moonData.isCalibrated) {
    DEBUG_PRINTLN("[MOON] Cannot update position - not calibrated");
    return false;
  }
  
  // Check if we need to increment to next moon cycle
  checkAndIncrementMoonCycle(currentEpoch);
  
  // Calculate target position
  float exactPhase = calculateExactMoonPhase(currentEpoch);
  int targetSteps = exactPhaseToSteps(exactPhase);
  
  // Calculate forward movement needed (unidirectional only)
  int stepsToMove = (MOON_STEPS_PER_REV + targetSteps - moonData.currentSteps) % MOON_STEPS_PER_REV;
  
  // Only move if significant change (avoid jitter)
  if (stepsToMove > 5) {
    DEBUG_PRINT("[MOON] Updating position: ");
    DEBUG_PRINT(stepsToMove);
    DEBUG_PRINTLN(" steps forward");
    
    moonStepper.setSpeed(MOON_SPEED_NORMAL);
    moonStepper.step(stepsToMove);
    disableMoonMotor();
    
    moonData.currentSteps = targetSteps;
    moonData.lastUpdate = millis();
  }
  
  // Update moon data structure
  moonData.lunarAge = calculateLunarAge(currentEpoch);
  moonData.phase = calculateMoonPhase(currentEpoch);
  moonData.exactPhase = exactPhase;
  moonData.illumination = calculateMoonIllumination(currentEpoch);
      
  // Check if monthly recalibration is needed
  if (checkAndRecalibrate(currentEpoch)) {
    DEBUG_PRINTLN("[MOON] Monthly recalibration completed");
  }
  
  return true;
}

/**
 * @brief Move moon to specific phase position
 * 
 * Moves motor to display specified phase. Uses unidirectional
 * rotation only (always moves forward).
 * 
 * @param phase Phase number (0-7)
 * @return true if movement successful
 */
bool moveMoonToPhase(uint8_t phase) {
  if (!moonData.isCalibrated) {
    return false;
  }
  
  if (phase >= MOON_PHASES_COUNT) {
    return false;
  }
  
  int targetSteps = phaseToSteps(phase);
  
  // Calculate forward movement (unidirectional)
  int stepsToMove = (MOON_STEPS_PER_REV + targetSteps - moonData.currentSteps) % MOON_STEPS_PER_REV;
  
  moonStepper.setSpeed(MOON_SPEED_NORMAL);
  moonStepper.step(stepsToMove);
  disableMoonMotor();
  
  moonData.currentSteps = targetSteps;
  moonData.lastUpdate = millis();
  
  return true;
}

// ==========================================
// ASTRONOMICAL CALCULATIONS (Hybrid Meeus + Average Cycle)
// ==========================================

/**
 * @brief Calculate lunar age using hybrid approach
 * 
 * Uses the precise next new moon reference (from Meeus or incremented)
 * to calculate days since last new moon.
 * 
 * @param currentEpoch Current Unix timestamp
 * @return Lunar age in days (0.0 to 29.53)
 */
float calculateLunarAge(unsigned long currentEpoch) {
  if (!moonData.meeusInitialized) {
    DEBUG_PRINTLN("[MOON] Warning: Meeus not initialized, returning 0");
    return 0.0;
  }
  
  // Calculate position within current lunar cycle
  // We know nextNewMoonEpoch, so work backwards to find current age
  long secondsUntilNewMoon = moonData.nextNewMoonEpoch - currentEpoch;
  float daysUntilNewMoon = secondsUntilNewMoon / 86400.0;
  
  // Lunar age = cycle length - days until new moon
  float lunarAge = MOON_LUNAR_CYCLE_DAYS - daysUntilNewMoon;
  
  // Handle case where we're past the new moon (shouldn't happen if checkAndIncrementMoonCycle is called)
  while (lunarAge < 0) {
    lunarAge += MOON_LUNAR_CYCLE_DAYS;
  }
  while (lunarAge >= MOON_LUNAR_CYCLE_DAYS) {
    lunarAge -= MOON_LUNAR_CYCLE_DAYS;
  }
  
  return lunarAge;
}

/**
 * @brief Calculate current moon phase (discrete 0-7)
 * 
 * Divides lunar cycle into 8 equal phases based on lunar age.
 * 
 * @param currentEpoch Current Unix timestamp
 * @return Phase number (0-7)
 */
uint8_t calculateMoonPhase(unsigned long currentEpoch) {
  float lunarAge = calculateLunarAge(currentEpoch);
  
  // Divide cycle into 8 equal phases
  float phaseSize = MOON_LUNAR_CYCLE_DAYS / 16.0;
  
  if (lunarAge < phaseSize)                     return 0;  // New Moon
  if (lunarAge < 3 * phaseSize)                 return 1;  // Waxing Crescent
  if (lunarAge < 5 * phaseSize)                 return 2;  // First Quarter
  if (lunarAge < 7 * phaseSize)                 return 3;  // Waxing Gibbous
  if (lunarAge < 9 * phaseSize)                 return 4;  // Full Moon
  if (lunarAge < 11 * phaseSize)                return 5;  // Waning Gibbous
  if (lunarAge < 13 * phaseSize)                return 6;  // Last Quarter
  if (lunarAge < 15 * phaseSize)                return 7;  // Waning Crescent
  
  return 0;  // Wrap around to New Moon
}

/**
 * @brief Calculate exact moon phase as continuous value
 * 
 * Returns phase as floating point for smooth motor positioning.
 * 
 * @param currentEpoch Current Unix timestamp
 * @return Exact phase (0.0 to 8.0)
 */
float calculateExactMoonPhase(unsigned long currentEpoch) {
  float lunarAge = calculateLunarAge(currentEpoch);
  float exactPhase = (lunarAge / MOON_LUNAR_CYCLE_DAYS) * 8.0;
  
  return exactPhase;
}

/**
 * @brief Calculate moon illumination percentage
 * 
 * Uses cosine function to approximate visible illumination.
 * 
 * @param currentEpoch Current Unix timestamp
 * @return Illumination percentage (0-100)
 */
float calculateMoonIllumination(unsigned long currentEpoch) {
  float lunarAge = calculateLunarAge(currentEpoch);
  float illumination = 50.0 * (1.0 - cos((lunarAge / MOON_LUNAR_CYCLE_DAYS) * 2.0 * PI));
  return illumination;
}

/**
 * @brief Get moon phase name in French
 * 
 * @param phase Phase number (0-7)
 * @return Phase name string
 */
const char* getMoonPhaseName(uint8_t phase) {
  if (phase >= MOON_PHASES_COUNT) {
    return "Unknown";
  }
  return MOON_PHASE_NAMES[phase];
}

// ==========================================
// MEEUS ALGORITHM IMPLEMENTATION
// ==========================================

/**
 * @brief Calculate next new moon using Jean Meeus algorithm
 * 
 * Implements the algorithm from "Astronomical Algorithms" by Jean Meeus (1998).
 * This provides precision of approximately ±2 minutes compared to NASA data.
 * 
 * The function:
 * 1. Converts current epoch to date
 * 2. Calculates new moon for current month
 * 3. If that moon is in the past, calculates next month
 * 4. Returns Unix timestamp of next new moon
 * 
 * @param currentEpoch Current Unix timestamp
 * @return Unix timestamp of next new moon (UTC)
 */
unsigned long calculateNextNewMoonMeeus(unsigned long currentEpoch) {
  // Convert epoch to date
  int year, month, day, hour, minute, second;
  epochToDateTime(currentEpoch, year, month, day, hour, minute, second);
  
  DEBUG_PRINT("[MOON] Calculating Meeus for date: ");
  DEBUG_PRINT(year);
  DEBUG_PRINT("/");
  DEBUG_PRINT(month);
  DEBUG_PRINT("/");
  DEBUG_PRINTLN(day);
  
  // Calculate new moon for current month
  double jd = calculateMeeusNewMoon(year, month);
  unsigned long newMoonEpoch = (unsigned long)((jd - JULIAN_EPOCH_OFFSET) * SECONDS_PER_DAY);
  
  // If this new moon is in the past, try next month
  if (newMoonEpoch <= currentEpoch) {
    month++;
    if (month > 12) {
      month = 1;
      year++;
    }
    jd = calculateMeeusNewMoon(year, month);
    newMoonEpoch = (unsigned long)((jd - JULIAN_EPOCH_OFFSET) * SECONDS_PER_DAY);
  }
  
  DEBUG_PRINT("[MOON] Meeus calculated next new moon epoch: ");
  DEBUG_PRINTLN(newMoonEpoch);
  
  return newMoonEpoch;
}

/**
 * @brief Core Meeus algorithm - Calculate new moon Julian Date
 * 
 * This is the mathematical heart of the Meeus algorithm. It calculates
 * the Julian Date of the new moon nearest to the given month using
 * astronomical formulae for lunar motion.
 * 
 * Algorithm steps:
 * 1. Calculate lunation number (k)
 * 2. Calculate mean new moon time (JDE)
 * 3. Apply corrections for:
 *    - Sun's mean anomaly (M)
 *    - Moon's mean anomaly (M')
 *    - Moon's argument of latitude (F)
 *    - Longitude of ascending node (Ω)
 *    - Earth's orbital eccentricity (E)
 *    - Planetary perturbations
 * 
 * @param year Year (e.g., 2025)
 * @param month Month (1-12)
 * @return Julian Date of new moon (decimal days)
 */
double calculateMeeusNewMoon(int year, int month) {
  // Calculate lunation number (k) - integer count of lunar cycles since J2000.0
  double k = (year + (month - 0.5) / 12.0 - 2000.0) * 12.3685;
  k = floor(k);
  
  // Time in Julian centuries since J2000.0
  double T = k / 1236.85;
  double T2 = T * T;
  double T3 = T2 * T;
  double T4 = T3 * T;
  
  // Mean new moon (JDE - Julian Ephemeris Day)
  double JDE = 2451550.09766 + 29.530588861 * k
               + 0.00015437 * T2
               - 0.000000150 * T3
               + 0.00000000073 * T4;
  
  // Sun's mean anomaly
  double M = 2.5534 + 29.10535670 * k
             - 0.0000014 * T2
             - 0.00000011 * T3;
  M = fmod(M, 360.0);
  
  // Moon's mean anomaly
  double Mprime = 201.5643 + 385.81693528 * k
                  + 0.0107582 * T2
                  + 0.00001238 * T3
                  - 0.000000058 * T4;
  Mprime = fmod(Mprime, 360.0);
  
  // Moon's argument of latitude
  double F = 160.7108 + 390.67050284 * k
             - 0.0016118 * T2
             - 0.00000227 * T3
             + 0.000000011 * T4;
  F = fmod(F, 360.0);
  
  // Longitude of ascending node
  double Omega = 124.7746 - 1.56375588 * k
                 + 0.0020672 * T2
                 + 0.00000215 * T3;
  Omega = fmod(Omega, 360.0);
  
  // Convert to radians for trigonometric functions
  double M_rad = M * PI / 180.0;
  double Mprime_rad = Mprime * PI / 180.0;
  double F_rad = F * PI / 180.0;
  double Omega_rad = Omega * PI / 180.0;
  
  // Earth's eccentricity correction
  double E = 1.0 - 0.002516 * T - 0.0000074 * T2;
  
  // Sum of periodic correction terms for new moon
  double correction = 0.0;
  
  // Main correction terms (in order of decreasing magnitude)
  correction += -0.40720 * sin(Mprime_rad);
  correction += 0.17241 * E * sin(M_rad);
  correction += 0.01608 * sin(2.0 * Mprime_rad);
  correction += 0.01039 * sin(2.0 * F_rad);
  correction += 0.00739 * E * sin(Mprime_rad - M_rad);
  correction += -0.00514 * E * sin(Mprime_rad + M_rad);
  correction += 0.00208 * E * E * sin(2.0 * M_rad);
  correction += -0.00111 * sin(Mprime_rad - 2.0 * F_rad);
  correction += -0.00057 * sin(Mprime_rad + 2.0 * F_rad);
  correction += 0.00056 * E * sin(2.0 * Mprime_rad + M_rad);
  correction += -0.00042 * sin(3.0 * Mprime_rad);
  correction += 0.00042 * E * sin(M_rad + 2.0 * F_rad);
  correction += 0.00038 * E * sin(M_rad - 2.0 * F_rad);
  correction += -0.00024 * E * sin(2.0 * Mprime_rad - M_rad);
  correction += -0.00017 * sin(Omega_rad);
  correction += -0.00007 * sin(Mprime_rad + 2.0 * M_rad);
  correction += 0.00004 * sin(2.0 * Mprime_rad - 2.0 * F_rad);
  correction += 0.00004 * sin(3.0 * M_rad);
  correction += 0.00003 * sin(Mprime_rad + M_rad - 2.0 * F_rad);
  correction += 0.00003 * sin(2.0 * Mprime_rad + 2.0 * F_rad);
  correction += -0.00003 * sin(Mprime_rad + M_rad + 2.0 * F_rad);
  correction += 0.00003 * sin(Mprime_rad - M_rad + 2.0 * F_rad);
  correction += -0.00002 * sin(Mprime_rad - M_rad - 2.0 * F_rad);
  correction += -0.00002 * sin(3.0 * Mprime_rad + M_rad);
  correction += 0.00002 * sin(4.0 * Mprime_rad);
  
  // Additional planetary perturbation terms
  double A1 = 299.77 + 0.107408 * k - 0.009173 * T2;
  A1 = fmod(A1, 360.0) * PI / 180.0;
  correction += 0.000325 * sin(A1);
  
  double A2 = 251.88 + 0.016321 * k;
  A2 = fmod(A2, 360.0) * PI / 180.0;
  correction += 0.000165 * sin(A2);
  
  double A3 = 251.83 + 26.651886 * k;
  A3 = fmod(A3, 360.0) * PI / 180.0;
  correction += 0.000164 * sin(A3);
  
  double A4 = 349.42 + 36.412478 * k;
  A4 = fmod(A4, 360.0) * PI / 180.0;
  correction += 0.000126 * sin(A4);
  
  double A5 = 84.66 + 18.206239 * k;
  A5 = fmod(A5, 360.0) * PI / 180.0;
  correction += 0.000110 * sin(A5);
  
  double A6 = 141.74 + 53.303771 * k;
  A6 = fmod(A6, 360.0) * PI / 180.0;
  correction += 0.000062 * sin(A6);
  
  double A7 = 207.14 + 2.453732 * k;
  A7 = fmod(A7, 360.0) * PI / 180.0;
  correction += 0.000060 * sin(A7);
  
  double A8 = 154.84 + 7.306860 * k;
  A8 = fmod(A8, 360.0) * PI / 180.0;
  correction += 0.000056 * sin(A8);
  
  double A9 = 34.52 + 27.261239 * k;
  A9 = fmod(A9, 360.0) * PI / 180.0;
  correction += 0.000047 * sin(A9);
  
  double A10 = 207.19 + 0.121824 * k;
  A10 = fmod(A10, 360.0) * PI / 180.0;
  correction += 0.000042 * sin(A10);
  
  double A11 = 291.34 + 1.844379 * k;
  A11 = fmod(A11, 360.0) * PI / 180.0;
  correction += 0.000040 * sin(A11);
  
  double A12 = 161.72 + 24.198154 * k;
  A12 = fmod(A12, 360.0) * PI / 180.0;
  correction += 0.000037 * sin(A12);
  
  double A13 = 239.56 + 25.513099 * k;
  A13 = fmod(A13, 360.0) * PI / 180.0;
  correction += 0.000035 * sin(A13);
  
  double A14 = 331.55 + 3.592518 * k;
  A14 = fmod(A14, 360.0) * PI / 180.0;
  correction += 0.000023 * sin(A14);
  
  // Apply correction to mean new moon time
  double JD = JDE + correction;
  
  return JD;
}

/**
 * @brief Check if new moon has passed and resync with Meeus
 * 
 * This function implements the hybrid approach with Meeus resynchronization
 * at the end of each lunar cycle:
 * - Checks if current time has passed the next new moon
 * - If yes, recalculates next new moon using Meeus algorithm (precise)
 * - Updates reference for next cycle
 * 
 * This ensures maximum accuracy by resynchronizing with Meeus at every
 * new moon, eliminating cumulative drift from using the average cycle.
 * 
 * @param currentEpoch Current Unix timestamp
 * @return true if cycle was incremented and Meeus recalculation performed
 */
bool checkAndIncrementMoonCycle(unsigned long currentEpoch) {
  if (!moonData.meeusInitialized) {
    return false;
  }
  
  // Check if we've passed the new moon
  if (currentEpoch >= moonData.nextNewMoonEpoch) {
    DEBUG_PRINTLN("[MOON] === New Moon Passed - Meeus Resynchronization ===");
    
    // Recalculate next new moon using Meeus algorithm
    moonData.nextNewMoonEpoch = calculateNextNewMoonMeeus(currentEpoch);
    moonData.lastMeeusSync = currentEpoch;
    
    DEBUG_PRINT("[MOON] Next new moon recalculated (Meeus): ");
    DEBUG_PRINTLN(moonData.nextNewMoonEpoch);
    DEBUG_PRINTLN("[MOON] ✓ Resync complete - precision maintained");
    
    return true;
  }
  
  return false;
}

// ==========================================
// DATE/TIME CONVERSION UTILITIES
// ==========================================

/**
 * @brief Convert Unix epoch to date/time components
 * 
 * Converts Unix timestamp (seconds since 1970-01-01 00:00:00 UTC)
 * into separate year, month, day, hour, minute, second components.
 * 
 * Handles leap years correctly using standard algorithm:
 * - Leap if divisible by 4
 * - Not leap if divisible by 100
 * - Leap if divisible by 400
 * 
 * @param epoch Unix timestamp
 * @param year Output: year (e.g., 2025)
 * @param month Output: month (1-12)
 * @param day Output: day (1-31)
 * @param hour Output: hour (0-23)
 * @param minute Output: minute (0-59)
 * @param second Output: second (0-59)
 */
void epochToDateTime(unsigned long epoch, int &year, int &month, int &day, int &hour, int &minute, int &second) {
  // Extract time components
  unsigned long remaining = epoch;
  second = remaining % 60;
  remaining /= 60;
  minute = remaining % 60;
  remaining /= 60;
  hour = remaining % 24;
  unsigned long days = remaining / 24;
  
  // Calculate date (starting from 1970)
  year = 1970;
  while (true) {
    int daysInYear = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 366 : 365;
    if (days >= daysInYear) {
      days -= daysInYear;
      year++;
    } else {
      break;
    }
  }
  
  // Calculate month and day
  int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
    daysInMonth[1] = 29;  // Leap year
  }
  
  month = 1;
  while (month <= 12) {
    if (days >= daysInMonth[month - 1]) {
      days -= daysInMonth[month - 1];
      month++;
    } else {
      break;
    }
  }
  
  day = days + 1;
}

// ==========================================
// SENSOR FUNCTIONS
// ==========================================

int readLDR() {
  long sum = 0;
  for (int i = 0; i < MOON_LDR_SAMPLE_COUNT; i++) {
    sum += analogRead(PIN_MOON_LDR_SENSOR);
    if (i < MOON_LDR_SAMPLE_COUNT - 1) {
      delay(MOON_LDR_SAMPLE_DELAY);
    }
  }
  return sum / MOON_LDR_SAMPLE_COUNT;
}

// ==========================================
// MOTOR CONTROL
// ==========================================

/**
 * @brief Disable stepper motor coils to prevent overheating
 * 
 * Cuts power to all motor coils. This prevents motor from overheating
 * during idle periods and reduces power consumption.
 * 
 * IMPORTANT: Call this after every motor movement!
 */
void disableMoonMotor() {
  digitalWrite(PIN_MOON_STEPPER_IN1, LOW);
  digitalWrite(PIN_MOON_STEPPER_IN2, LOW);
  digitalWrite(PIN_MOON_STEPPER_IN3, LOW);
  digitalWrite(PIN_MOON_STEPPER_IN4, LOW);
}

/**
 * @brief Convert phase number to motor steps
 * 
 * Converts discrete phase (0-7) to motor position in steps.
 * Phase 0 (New Moon) = 0 steps (home position)
 * Phase 4 (Full Moon) = 1024 steps (180° rotation)
 * 
 * @param phase Phase number (0-7)
 * @return Motor position in steps (0 to MOON_STEPS_PER_REV-1)
 */
int phaseToSteps(uint8_t phase) {
  return (phase * MOON_STEPS_PER_REV) / 8;
}

/**
 * @brief Convert exact phase to stepper motor steps (continuous)
 * 
 * Converts floating-point phase value to motor steps for smooth positioning.
 * Unlike phaseToSteps(), this provides continuous positioning within phases.
 * 
 * Examples:
 * - exactPhase 0.0 → 0 steps (New Moon)
 * - exactPhase 2.5 → 640 steps (between First Quarter and Waxing Gibbous)
 * - exactPhase 4.0 → 1024 steps (Full Moon)
 * - exactPhase 7.5 → 1920 steps (near end of Waning Crescent)
 * 
 * @param exactPhase Exact phase (0.0 to 8.0)
 * @return Number of steps from home position (0 to MOON_STEPS_PER_REV-1)
 */
int exactPhaseToSteps(float exactPhase) {
  // Convert continuous phase to steps
  int steps = (int)((exactPhase / 8.0) * MOON_STEPS_PER_REV);
  
  // Keep within valid range (wrap around if needed)
  steps = steps % MOON_STEPS_PER_REV;
  if (steps < 0) steps += MOON_STEPS_PER_REV;
  
  return steps;
}

// ==========================================
// STATUS AND DEBUGGING
// ==========================================

MoonPhaseData& getMoonData() {
  return moonData;
}

float daysSinceLastCalibration(unsigned long currentEpoch) {
  if (moonData.lastCalib == 0) {
    return 0.0;
  }
  
  long secondsSinceCalib = currentEpoch - moonData.lastCalib;
  return secondsSinceCalib / 86400.0;
}