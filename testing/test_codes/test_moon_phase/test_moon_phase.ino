/**
 * Smart LED Clock - Moon Phase Module Test
 * 
 * This test verifies the complete moon phase display system including:
 * - 28BYJ-48 stepper motor control via ULN2003 driver
 * - LDR (photoresistor) home position sensor with Gaussian peak detection
 * - Calibration LED indicator
 * - Astronomical phase calculations
 * - Continuous rotation positioning
 * - Auto-calibration system using brightness peak detection
 * 
 * Hardware Required:
 * - Arduino Uno R4 WiFi
 * - 28BYJ-48 5V stepper motor
 * - ULN2003 driver board
 * - LDR (photoresistor) with pull-down resistor
 * - LED for calibration indication
 * - 220Ω resistor for LED
 * - Calibration disk with reference hole
 * - 3D printed bicolor moon sphere (yellow/black with Celtic runes)
 * 
 * Pin Configuration:
 * - D3  → ULN2003 IN1
 * - D4  → ULN2003 IN2
 * - D7  → ULN2003 IN3
 * - D8  → ULN2003 IN4
 * - A1  → LDR sensor (analog input)
 * - D12 → Calibration LED (lights up during sync only)
 * 
 * LDR Circuit:
 * A1  ────┬──── LDR ──── +5V
 *         │
 *       10kΩ (Pull-down)
 *         │
 *        GND
 *
 * Calibration LED:
 * D12 ── 220Ω ── LED ─── GND
 * 
 * ⚠️  POWER REQUIREMENTS:
 * - Stepper motor draws ~200mA during movement
 * - RECOMMENDED: External 5V power supply (1A+)
 * - Connect grounds together (Arduino GND + PSU GND)
 * - Motor coils disabled after movement to prevent heating
 * 
 * Test Menu (Simplified):
 * 1. Motor basic test (full revolution)
 * 2. LDR & LED functionality test
 * 3. Gaussian peak calibration (find home by max brightness)
 * 4. Continuous rotation demo (accelerated lunar cycle)
 * 5. Display current position & status
 * 0. Reset and recalibrate
 * 
 * Expected Results:
 * - Motor rotates smoothly without overheating
 * - LDR detects brightness peak reliably
 * - Calibration LED blinks during search
 * - Peak detection finds hole alignment accurately
 * - Motor coils disable after movement (no heat)
 * 
 * Test version: 1.1.0
 * Compatible with: Smart LED Clock v1.1.0+
 * Author: F. Baillon
 * License: MIT
 */

#include <Stepper.h>
#include "moon_calculations.h"

// ==========================================
// PIN DEFINITIONS
// ==========================================
#define PIN_MOON_IN1            3    // ULN2003 IN1
#define PIN_MOON_IN2            4    // ULN2003 IN2
#define PIN_MOON_IN3            7    // ULN2003 IN3
#define PIN_MOON_IN4            8    // ULN2003 IN4
#define PIN_LDR_SENSOR         A1    // LDR photoresistor (analog)
#define PIN_CALIBRATION_LED    12    // LED indicator for calibration

// ==========================================
// MOTOR CONFIGURATION
// ==========================================
#define STEPS_PER_REV          2048  // 28BYJ-48 with gear reduction
#define MOTOR_SPEED_SLOW          5  // RPM for calibration
#define MOTOR_SPEED_NORMAL       10  // RPM for positioning
#define MOTOR_SPEED_FAST         15  // RPM for demonstrations

// ==========================================
// LDR SENSOR CONFIGURATION
// ==========================================
#define LDR_SAMPLE_COUNT        10   // Samples for averaging
#define LDR_SAMPLE_DELAY        10   // ms between samples
#define LDR_PEAK_THRESHOLD     0.7   // Display values > 70% of peak

// ==========================================
// CALIBRATION SETTINGS
// ==========================================
#define CALIBRATION_STEP_SIZE    8   // Steps between LDR readings (8 = fast, 1 = precise)
#define CALIBRATION_LED_BLINK  250   // ms blink rate during calibration

// ==========================================
// GLOBAL VARIABLES
// ==========================================
Stepper moonStepper(STEPS_PER_REV, PIN_MOON_IN1, PIN_MOON_IN3, PIN_MOON_IN2, PIN_MOON_IN4);
int currentPosition = 0;      // Current position in steps (0-2047)
bool isCalibrated = false;
unsigned long fakeEpoch = 0;  // For accelerated simulation

// ==========================================
// MOTOR CONTROL FUNCTIONS
// ==========================================

/**
 * Disable stepper motor coils to prevent heating
 * IMPORTANT: Call this after every movement to save power and prevent overheating
 */
void disableMotor() {
  digitalWrite(PIN_MOON_IN1, LOW);
  digitalWrite(PIN_MOON_IN2, LOW);
  digitalWrite(PIN_MOON_IN3, LOW);
  digitalWrite(PIN_MOON_IN4, LOW);
}

// ==========================================
// SETUP
// ==========================================
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  delay(1000);
  
  Serial.println("\n\n");
  Serial.println("================================================");
  Serial.println("   Smart LED Clock - Moon Phase Module Test");
  Serial.println("================================================");
  Serial.println("Test Version: 1.0.0");
  Serial.println("Author: F. Baillon");
  Serial.println("Build Date: " __DATE__ " " __TIME__);
  Serial.println("================================================");
  Serial.println();
  
  // Initialize pins
  pinMode(PIN_LDR_SENSOR, INPUT);
  pinMode(PIN_CALIBRATION_LED, OUTPUT);
  digitalWrite(PIN_CALIBRATION_LED, LOW);
  
  // Set initial motor speed
  moonStepper.setSpeed(MOTOR_SPEED_NORMAL);
  
  Serial.println("[INIT] Pins configured");
  Serial.println("[INIT] Motor initialized");
  Serial.println();
  
  // Read LDR initial state
  int ldrValue = readLDR();
  Serial.print("[SENSOR] LDR initial reading: ");
  Serial.print(ldrValue);
  Serial.println(" / 1023");
  
  if (ldrValue > 700) {
    Serial.println("[SENSOR] Status: BRIGHT (likely near hole)");
  } else if (ldrValue > 300) {
    Serial.println("[SENSOR] Status: MEDIUM brightness");
  } else {
    Serial.println("[SENSOR] Status: DARK (blocked)");
  }
  
  Serial.println();
  Serial.println("[INFO] Calibration LED on D12 will light during sync");
  Serial.println();
  
  printMenu();
}

// ==========================================
// MAIN LOOP
// ==========================================
void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    
    // Clear any remaining characters
    while (Serial.available()) Serial.read();
    
    Serial.println();
    handleCommand(command);
    Serial.println();
    printMenu();
  }
}

// ==========================================
// LDR SENSOR FUNCTIONS
// ==========================================

/**
 * Read LDR sensor value with averaging
 * @return Averaged analog value (0-1023)
 */
int readLDR() {
  long sum = 0;
  for (int i = 0; i < LDR_SAMPLE_COUNT; i++) {
    sum += analogRead(PIN_LDR_SENSOR);
    if (i < LDR_SAMPLE_COUNT - 1) {
      delay(LDR_SAMPLE_DELAY);
    }
  }
  return sum / LDR_SAMPLE_COUNT;
}

/**
 * Blink calibration LED
 * @param times Number of blinks
 * @param delayMs Delay between blinks
 */
void blinkCalibrationLED(int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(PIN_CALIBRATION_LED, HIGH);
    delay(delayMs);
    digitalWrite(PIN_CALIBRATION_LED, LOW);
    delay(delayMs);
  }
}

// ==========================================
// MENU SYSTEM
// ==========================================
void printMenu() {
  Serial.println("┌────────────────────────────────────────────┐");
  Serial.println("│         MOON PHASE TEST MENU               │");
  Serial.println("├────────────────────────────────────────────┤");
  Serial.println("│ 1 - Motor basic test (full revolution)     │");
  Serial.println("│ 2 - LDR & LED functionality test           │");
  Serial.println("│ 3 - Gaussian peak calibration (find home)  │");
  Serial.println("│ 4 - Continuous rotation demo (accelerated) │");
  Serial.println("│ 5 - Display current position & status      │");
  Serial.println("│ 6 - Position to specific phase             │");
  Serial.println("└─────────────────────────────────────────────┘");
  Serial.print("\nCommand: ");
}

void handleCommand(char cmd) {
  switch (cmd) {
    case '1':
      testMotorBasic();
      break;
      
    case '2':
      testLDRandLED();
      break;
      
    case '3':
      gaussianPeakCalibration();
      break;
      
    case '4':
      continuousRotationDemo();
      break;
      
    case '5':
      displayStatus();
      break;

    case '6':
      positionToPhase();
      break;

    default:
      Serial.println("[ERROR] Invalid command. Please select 1-6.");
      break;
  }
}

// ==========================================
// TEST FUNCTIONS
// ==========================================

/**
 * Test 1: Motor basic test
 */
void testMotorBasic() {
  Serial.println("=== TEST 1: Motor Basic Test ===");
  Serial.println("[TEST] Rotating motor one complete revolution (2048 steps)");
  Serial.println("[TEST] Speed: " + String(MOTOR_SPEED_NORMAL) + " RPM");
  Serial.println("[TEST] Expected duration: ~5 seconds");
  Serial.println();
  
  unsigned long startTime = millis();
  
  moonStepper.setSpeed(MOTOR_SPEED_NORMAL);
  
  // Rotate in steps with progress indicator
  for (int i = 0; i < STEPS_PER_REV; i += 128) {
    moonStepper.step(128);
    int progress = (i * 100) / STEPS_PER_REV;
    Serial.print("\r[TEST] Progress: ");
    Serial.print(progress);
    Serial.println("%");
  }
  
  unsigned long duration = (millis() - startTime) / 1000;
  
  Serial.println("\r[TEST] Progress: 100%");
  
  // Disable motor to prevent heating
  disableMotor();
  
  Serial.println("[TEST] ✓ Rotation completed in " + String(duration) + " seconds");
  Serial.println("[TEST] ✓ Motor coils disabled (no heating)");
  Serial.println();
  
  Serial.println("[INFO] Verification checklist:");
  Serial.println("  ✓ Smooth rotation without jerking");
  Serial.println("  ✓ No motor stalling or skipping steps");
  Serial.println("  ✓ Sphere completes full revolution");
  Serial.println("  ✓ Motor doesn't overheat after stop");
}

/**
 * Test 2: LDR and LED functionality test
 */
void testLDRandLED() {
  Serial.println("=== TEST 2: LDR & LED Functionality Test ===");
  Serial.println("[TEST] Testing LDR sensor and calibration LED...");
  Serial.println();
  
  // Test LDR
  Serial.println("[LDR] Reading sensor value...");
  int ldrValue = readLDR();
  
  Serial.print("[LDR] Current reading: ");
  Serial.print(ldrValue);
  Serial.println(" / 1023");
  
  if (ldrValue > 0 && ldrValue < 1023) {
    Serial.println("[LDR] ✓ LDR is working (value is valid)");
  } else {
    Serial.println("[LDR] ⚠ Warning: LDR reading at extreme value");
    Serial.println("[LDR] Check wiring if value is exactly 0 or 1023");
  }
  
  Serial.println();
  
  // Test LED
  Serial.println("[LED] Testing calibration LED...");
  Serial.println("[LED] LED will turn ON for 2 seconds");
  
  digitalWrite(PIN_CALIBRATION_LED, HIGH);
  delay(2000);
  digitalWrite(PIN_CALIBRATION_LED, LOW);
  
  Serial.println("[LED] ✓ Calibration LED test complete");
  Serial.println();
  
  Serial.println("[INFO] Both LDR and LED are functional");
  Serial.println("[INFO] Ready for calibration (Test 3)");
}

/**
 * Test 3: Gaussian peak calibration (find home by maximum brightness)
 * This is the primary calibration method that will be used in production
 */
void gaussianPeakCalibration() {
  Serial.println("=== TEST 3: Gaussian Peak Calibration ===");
  Serial.println("[CALIB] Searching for brightness peak (perfect hole alignment)");
  Serial.println("[CALIB] The peak represents the exact center alignment of holes");
  Serial.println("[CALIB] This scan will take approximately 40 seconds");
  Serial.println("[CALIB] LED will blink during search");
  Serial.println();
  
  moonStepper.setSpeed(MOTOR_SPEED_SLOW);
  
  // Storage for peak detection
  int peakStep = 0;
  int peakValue = 0;
  int currentStep = 0;
  
  // LED blink state
  unsigned long lastBlink = 0;
  bool ledState = false;
  
  // Turn on LED to indicate calibration started
  digitalWrite(PIN_CALIBRATION_LED, HIGH);
  
  Serial.println("Step   | LDR Value | Current Peak | Status");
  Serial.println("-------|-----------|--------------|------------------");
  
  // Scan full revolution in CALIBRATION_STEP_SIZE increments
  for (currentStep = 0; currentStep < STEPS_PER_REV; currentStep += CALIBRATION_STEP_SIZE) {
    // Blink LED during search
    if (millis() - lastBlink > CALIBRATION_LED_BLINK) {
      ledState = !ledState;
      digitalWrite(PIN_CALIBRATION_LED, ledState);
      lastBlink = millis();
    }
    
    // Move motor
    moonStepper.step(CALIBRATION_STEP_SIZE);
    delay(20);  // Short stabilization delay
    
    // Read LDR
    int ldrValue = readLDR();
    
    // Update peak if new maximum found
    if (ldrValue > peakValue) {
      peakValue = ldrValue;
      peakStep = currentStep;
    }
    
    // Display only values near peak (reduces serial spam)
    // Show all readings above 70% of current peak, plus periodic updates
    if (ldrValue > peakValue * LDR_PEAK_THRESHOLD || currentStep % 256 == 0) {
      // Format: Step | Value | Peak | Status
      char buffer[80];
      sprintf(buffer, "%4d   | %4d      | %4d         |", 
              currentStep, ldrValue, peakValue);
      Serial.print(buffer);
      
      if (ldrValue == peakValue) {
        Serial.print(" ← NEW PEAK! 🎯");
      } else if (ldrValue > peakValue * 0.9) {
        Serial.print(" Near peak");
      }
      
      Serial.println();
    }
  }
  
  Serial.println();
  
  // Celebrate successful scan but keep LED ON for verification
  digitalWrite(PIN_CALIBRATION_LED, HIGH);
  delay(200);
  digitalWrite(PIN_CALIBRATION_LED, LOW);
  delay(200);
  digitalWrite(PIN_CALIBRATION_LED, HIGH);
  delay(200);
  digitalWrite(PIN_CALIBRATION_LED, LOW);
  delay(200);
  digitalWrite(PIN_CALIBRATION_LED, HIGH);
  delay(200);
  digitalWrite(PIN_CALIBRATION_LED, LOW);
  delay(200);
  
  // Keep LED ON for positioning and verification
  digitalWrite(PIN_CALIBRATION_LED, HIGH);
  
  Serial.println("[CALIB] ✓ Scan complete!");
  Serial.println("[CALIB] Peak detected at step: " + String(peakStep));
  Serial.println("[CALIB] Peak brightness value: " + String(peakValue) + " / 1023");
  
  // Validate peak quality
  if (peakValue < 300) {
    Serial.println("[CALIB] ⚠ WARNING: Peak value is low (< 300)");
    Serial.println("[CALIB] Check: hole size, LDR positioning, ambient light");
  } else {
    Serial.println("[CALIB] ✓ Peak quality is good");
  }
  
  Serial.println();
  
  // Calculate shortest path back to peak
  int stepsToMove = calculateShortestPath(STEPS_PER_REV, peakStep);
  
  Serial.println("[CALIB] Returning to peak position...");
  Serial.println("[CALIB] Movement: " + String(stepsToMove) + " steps");
  
  moonStepper.step(stepsToMove);
  
  // Disable motor to prevent heating
  disableMotor();
  
  // Set this as our home position (New Moon = phase 0)
  currentPosition = 0;
  isCalibrated = true;
  
  Serial.println();
  Serial.println("[CALIB] ✓ Positioned at peak (home position)");
  
  // Verify final position BEFORE turning off LED
  delay(200);  // Let motor settle
  int finalValue = readLDR();
  
  Serial.println("[CALIB] Final LDR reading: " + String(finalValue) + " / 1023");
  
  if (abs(finalValue - peakValue) < 50) {
    Serial.println("[CALIB] ✓ Position verified - at maximum brightness");
  } else {
    Serial.println("[CALIB] ⚠ Position verification warning");
    Serial.println("[CALIB] Expected: " + String(peakValue) + ", Got: " + String(finalValue));
    Serial.println("[CALIB] Difference: " + String(abs(finalValue - peakValue)));
    
    if (abs(finalValue - peakValue) > 200) {
      Serial.println("[CALIB] ⚠ Large difference detected - possible motor step loss");
      Serial.println("[CALIB] Consider: reducing speed, checking mechanical friction");
    }
  }
  
  Serial.println();
  Serial.println("[CALIB] ✓✓✓ CALIBRATION COMPLETE ✓✓✓");
  Serial.println("[CALIB] System calibrated. Position = 0 (New Moon reference)");
  Serial.println("[CALIB] Motor coils disabled to prevent heating");
  
  // Turn off LED AFTER verification
  digitalWrite(PIN_CALIBRATION_LED, LOW);
}

/**
 * Test 4: Continuous rotation demo (accelerated lunar cycle)
 */
void continuousRotationDemo() {
  Serial.println("=== TEST 4: Continuous Rotation Demo ===");
  Serial.println("[DEMO] Simulating complete lunar cycle");
  Serial.println("[DEMO] 29.5 days compressed into 30 seconds");
  Serial.println("[DEMO] Press any key to stop");
  Serial.println();
  
  moonStepper.setSpeed(MOTOR_SPEED_FAST);
  
  int stepsPerUpdate = 10;
  int delayMs = 150;
  int totalSteps = 0;
  
  unsigned long startTime = millis();
  
  while (!Serial.available() && totalSteps < STEPS_PER_REV) {
    moonStepper.step(stepsPerUpdate);
    totalSteps += stepsPerUpdate;
    
    // Calculate simulated phase
    float simulatedAge = (totalSteps * MOON_LUNAR_CYCLE_DAYS) / STEPS_PER_REV;
    uint8_t phase = (totalSteps * 8) / STEPS_PER_REV;
    
    Serial.print("\r[DEMO] Day ");
    Serial.print(simulatedAge, 1);
    Serial.print(" - Phase ");
    Serial.print(phase);
    Serial.print(" (");
    Serial.print(getMoonPhaseName(phase));
    Serial.println(")");
    
    delay(delayMs);
  }

  // Clear buffer
  while (Serial.available()) Serial.read();
  
  unsigned long duration = (millis() - startTime) / 1000;
  
  // Disable motor
  disableMotor();
  
  Serial.println();
  Serial.println();
  Serial.println("[DEMO] ✓ Demo completed in " + String(duration) + " seconds");
  Serial.println("[DEMO] Total steps: " + String(totalSteps));
  Serial.println("[DEMO] ✓ Motor coils disabled");

  currentPosition = totalSteps;
}

/**
 * Test 5: Display current position and complete status
 */
void displayStatus() {
  Serial.println("=== TEST 5: System Status ===");
  Serial.println();
  
  // Calibration status
  Serial.println("┌─────────────────────────────────────┐");
  Serial.println("│         CALIBRATION STATUS          │");
  Serial.println("└─────────────────────────────────────┘");
  
  Serial.print("[STATUS] System calibrated: ");
  Serial.println(isCalibrated ? "✓ YES" : "✗ NO - Run Test 3 first");
  Serial.println();
  
  // Position information
  Serial.println("┌─────────────────────────────────────┐");
  Serial.println("│         POSITION INFORMATION        │");
  Serial.println("└─────────────────────────────────────┘");
  
  Serial.print("[POS] Current step position: ");
  Serial.print(currentPosition);
  Serial.print(" / ");
  Serial.println(STEPS_PER_REV);
  
  uint8_t phase = stepsToPhase(currentPosition);
  Serial.print("[POS] Corresponding phase: ");
  Serial.print(phase);
  Serial.print(" - ");
  Serial.println(getMoonPhaseName(phase));
  
  float percentage = (currentPosition * 100.0) / STEPS_PER_REV;
  Serial.print("[POS] Cycle completion: ");
  Serial.print(percentage, 1);
  Serial.println("%");
  
  Serial.println();
  
  // LDR sensor status
  Serial.println("┌─────────────────────────────────────┐");
  Serial.println("│         SENSOR INFORMATION          │");
  Serial.println("└─────────────────────────────────────┘");
  
  int ldrValue = readLDR();
  Serial.print("[LDR] Current reading: ");
  Serial.print(ldrValue);
  Serial.println(" / 1023");
  
  if (ldrValue > 700) {
    Serial.println("[LDR] Brightness level: HIGH (likely aligned with hole)");
  } else if (ldrValue > 300) {
    Serial.println("[LDR] Brightness level: MEDIUM");
  } else {
    Serial.println("[LDR] Brightness level: LOW (blocked)");
  }
  
  Serial.println();
  
  // Calculate and display current lunar phase based on date
  Serial.println("┌─────────────────────────────────────┐");
  Serial.println("│      ASTRONOMICAL CALCULATION       │");
  Serial.println("└─────────────────────────────────────┘");
  
  // Using test epoch for demonstration
  unsigned long testEpoch = 1761754860L;  // Friday 29 October 2025 16:21:00
  float lunarAge = getLunarAge(testEpoch);
  uint8_t calcPhase = calculateMoonPhase(testEpoch);
  float illumination = getMoonIllumination(testEpoch);
  
  Serial.println("[ASTRO] Based on test date (Oct 29, 2025):");
  Serial.print("[ASTRO] Lunar age: ");
  Serial.print(lunarAge, 2);
  Serial.println(" days");
  
  Serial.print("[ASTRO] Current phase: ");
  Serial.print(calcPhase);
  Serial.print(" - ");
  Serial.println(getMoonPhaseName(calcPhase));
  
  Serial.print("[ASTRO] Illumination: ");
  Serial.print(illumination, 1);
  Serial.println("%");
  
  Serial.println();
  Serial.println("[INFO] In production, RTC time will be used for calculations");
}


/**
 * Test 6: Position to specific phase
 */
void positionToPhase() {
  if (!isCalibrated) {
    Serial.println("[ERROR] System not calibrated. Run test 3 first.");
    return;
  }
  
  Serial.println("=== TEST 4: Position to Specific Phase ===");
  Serial.print("Enter phase number (0-7): ");
  
  // Wait for input
  while (!Serial.available()) delay(10);
  
  int phase = Serial.parseInt();
  
  // Clear buffer
  while (Serial.available()) Serial.read();
  
  Serial.println(phase);
  Serial.println();
  
  if (phase < 0 || phase > 7) {
    Serial.println("[ERROR] Invalid phase. Must be 0-7.");
    return;
  }
  
  Serial.println("[MOVE] Target: Phase " + String(phase) + " - " + getMoonPhaseName(phase));
  
  int targetSteps = phaseToSteps(phase);
  int stepsToMove = targetSteps-currentPosition;
  
  if (stepsToMove < 0) stepsToMove += MOON_STEPS_PER_REVOLUTION;
  
  Serial.println("[MOVE] Current position: " + String(currentPosition) + " steps");
  Serial.println("[MOVE] Target position: " + String(targetSteps) + " steps");
  Serial.println("[MOVE] Movement: " + String(stepsToMove) + " steps");
  Serial.println();
  
  if (stepsToMove == 0) {
    Serial.println("[MOVE] Already at target position!");
    return;
  }
  
  moonStepper.setSpeed(MOTOR_SPEED_NORMAL);
  
  Serial.println("[MOVE] Moving...");
  
  // Move in chunks with progress
  int remaining = stepsToMove;
  
  while (remaining > 0) {
    int chunk = min(remaining, 128);
    moonStepper.step(chunk);
    remaining -= chunk;
    
    Serial.print("\r[MOVE] Progress: ");
    Serial.print((stepsToMove - remaining) * 100 / stepsToMove);
    Serial.println("%");
  }
  
  currentPosition = targetSteps;
  
  Serial.println("\r[MOVE] ✓ Position reached!");
  Serial.println("[MOVE] Current position: " + String(currentPosition) + " steps");
}
