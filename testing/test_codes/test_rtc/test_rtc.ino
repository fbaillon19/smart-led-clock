/**
 * Smart LED Clock - DS3231 RTC Module Test
 * 
 * This test verifies that the DS3231 Real-Time Clock module is working correctly:
 * - I2C communication with DS3231
 * - Time reading and setting
 * - Temperature sensor reading
 * - Battery backup status
 * - Alarm functionality (if used)
 * - Accuracy over time
 * - **NEW: Square Wave (SQW) 1Hz interrupt for precise timing**
 * 
 * Expected Results:
 * - DS3231 should be detected on I2C bus (address 0x68)
 * - Time should read correctly and advance properly
 * - Temperature sensor should provide reasonable values
 * - Battery backup should be functional
 * - Time should maintain accuracy even after power cycles
 * - **SQW interrupt should trigger precisely every second**
 * 
 * Hardware Setup:
 * - Connect DS3231 SQW pin to Arduino D2
 * - Button is now on D13 (moved from D2)
 * 
 * Author: F. Baillon
 * License: MIT
 */

#include <Wire.h>
#include <RTClib.h>

// ==========================================
// CONFIGURATION
// ==========================================

/**
 * Pin pour interruption SQW du DS3231
 * La broche SQW du DS3231 doit être connectée à D2
 */
#define PIN_DS3231_SQW    2

// ==========================================
// GLOBAL VARIABLES
// ==========================================

// DS3231 RTC object
RTC_DS3231 rtc;

// Test tracking
unsigned long testStartTime;
DateTime lastTimeReading;
int testCount = 0;
bool rtcInitialized = false;

// Status tracking
bool batteryGood = true;
float temperatureSum = 0.0;
int temperatureReadings = 0;
long timeDriftSeconds = 0;

// ==========================================
// INTERRUPT VARIABLES (volatile)
// ==========================================

/**
 * Variables modifiées par l'interruption
 * DOIVENT être déclarées volatile
 */
volatile bool secondTicked = false;        // Flag: une seconde s'est écoulée
volatile unsigned long tickCount = 0;      // Compteur total de ticks
volatile unsigned long lastTickMillis = 0; // Timestamp du dernier tick

// ==========================================
// INTERRUPT SERVICE ROUTINE
// ==========================================

/**
 * Fonction appelée par l'interruption à chaque front descendant du SQW
 * DOIT être TRÈS RAPIDE (pas de Serial.print, pas de delay)
 */
void onSecondTick() {
  secondTicked = true;
  tickCount++;
  lastTickMillis = millis();
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=====================================");
  Serial.println("   Smart LED Clock - DS3231 RTC Test");
  Serial.println("         WITH SQW INTERRUPT");
  Serial.println("=====================================");
  Serial.println();
  
  // Configuration du pin SQW
  pinMode(PIN_DS3231_SQW, INPUT_PULLUP);
  Serial.print("SQW interrupt pin configured: D");
  Serial.println(PIN_DS3231_SQW);
  
  // LED intégrée pour visualisation des ticks
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("LED_BUILTIN will blink at each interrupt");
  
  Wire.begin();
  Serial.println("I2C bus initialized");
  
  scanI2CDevices();
  
  Serial.println("Initializing DS3231 RTC module...");
  
  if (!rtc.begin()) {
    Serial.println("❌ CRITICAL ERROR: DS3231 RTC not found!");
    Serial.println("Check connections:");
    Serial.println("  VCC -> 5V (or 3.3V)");
    Serial.println("  GND -> GND");
    Serial.println("  SDA -> A4");
    Serial.println("  SCL -> A5");
    Serial.println("  SQW -> D2");
    Serial.println("\nTest cannot continue without DS3231!");
    while(1) delay(1000);
  }
  
  rtcInitialized = true;
  Serial.println("✅ DS3231 RTC initialized successfully");
  
  if (rtc.lostPower()) {
    Serial.println("⚠️  WARNING: DS3231 lost power! Time may be incorrect");
    Serial.println("Setting time to compile time...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("Time has been set to compile time");
  } else {
    Serial.println("✅ DS3231 maintained time through power cycles");
  }
  
  // ==========================================
  // CONFIGURATION SQW à 1Hz
  // ==========================================
  Serial.println();
  Serial.println("Configuring DS3231 for 1Hz square wave output...");
  rtc.writeSqwPinMode(DS3231_SquareWave1Hz);
  delay(100);  // Laisser le temps au DS3231 de configurer
  
  // Vérifier l'état initial du pin SQW
  int sqwState = digitalRead(PIN_DS3231_SQW);
  Serial.print("SQW pin initial state: ");
  Serial.println(sqwState ? "HIGH" : "LOW");
  
  // Attacher l'interruption
  Serial.println("Attaching interrupt to SQW pin...");
  attachInterrupt(digitalPinToInterrupt(PIN_DS3231_SQW), onSecondTick, FALLING);
  Serial.println("✅ Interrupt attached successfully");
  Serial.println();
  
  DateTime now = rtc.now();
  lastTimeReading = now;
  testStartTime = millis();
  
  Serial.println("\nDS3231 RTC Test Started");
  Serial.println("Tests will run continuously...");
  Serial.println("Send 's' for status, 'i' for interrupt stats, 'r' to reset time");
  Serial.println("Send 't' for temperature, 'x' for power test, 'h' for help");
  Serial.println();
  
  printCurrentStatus();
}

void loop() {
  // ==========================================
  // GESTION INTERRUPTION (prioritaire)
  // ==========================================
  static bool ledState = false;
  
  if (secondTicked) {
    // Reset du flag IMMÉDIATEMENT
    secondTicked = false;
    
    // Basculer LED pour visualisation
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
    
    // Tests RTC déclenchés par l'interruption
    runRTCTests();
    testCount++;
  }
  
  // ==========================================
  // COMMANDES SÉRIE
  // ==========================================
  handleSerialCommands();
  
  // ==========================================
  // STATISTIQUES PÉRIODIQUES
  // ==========================================
  static unsigned long lastDetailedStatus = 0;
  if (millis() - lastDetailedStatus >= 30000) {
    printDetailedStatus();
    printInterruptStats();
    lastDetailedStatus = millis();
  }
  
  delay(10);
}

void scanI2CDevices() {
  Serial.println("Scanning I2C bus for devices...");
  
  byte error, address;
  int deviceCount = 0;
  
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      
      if (address == 0x68) {
        Serial.print(" (DS3231 RTC)");
      } else if (address == 0x27 || address == 0x3F) {
        Serial.print(" (LCD Display)");
      }
      Serial.println();
      deviceCount++;
    }
  }
  
  if (deviceCount == 0) {
    Serial.println("❌ No I2C devices found!");
  } else {
    Serial.print("✅ Found ");
    Serial.print(deviceCount);
    Serial.println(" I2C device(s)");
  }
  Serial.println();
}

void runRTCTests() {
  if (!rtcInitialized) return;
  
  DateTime now = rtc.now();
  
  if (testCount > 0) {
    long actualDiff = now.unixtime() - lastTimeReading.unixtime();
    long expectedInterval = 1;
    long driftThisReading = actualDiff - expectedInterval;
    
    if (abs(driftThisReading) > 2) {
      Serial.println("⚠️  WARNING: Significant time jump detected!");
      Serial.print("Expected interval: ");
      Serial.print(expectedInterval);
      Serial.print("s, Actual interval: ");
      Serial.print(actualDiff);
      Serial.println("s");
    }
    
    static long totalExpectedTime = 0;
    static long totalActualTime = 0;
    totalExpectedTime += 1;
    totalActualTime += actualDiff;
    timeDriftSeconds = totalActualTime - totalExpectedTime;
  }
  
  float temperature = rtc.getTemperature();
  if (!isnan(temperature) && temperature > -50 && temperature < 85) {
    temperatureSum += temperature;
    temperatureReadings++;
  }
  
  if (testCount % 10 == 0) {
    Serial.print("Time: ");
    printDateTime(now);
    Serial.print(" | Temp: ");
    Serial.print(temperature);
    Serial.print("°C | Cumulative drift: ");
    Serial.print(timeDriftSeconds);
    Serial.println("s");
  }
  
  lastTimeReading = now;
}

void handleSerialCommands() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    
    switch (command) {
      case 's':
      case 'S':
        printCurrentStatus();
        break;
        
      case 'i':
      case 'I':
        printInterruptStats();
        break;
        
      case 'r':
      case 'R':
        resetTimeToCompileTime();
        break;
        
      case 't':
      case 'T':
        printTemperatureInfo();
        break;
        
      case 'd':
      case 'D':
        printDetailedDiagnostics();
        break;
        
      case 'b':
      case 'B':
        checkBatteryStatus();
        break;
        
      case 'x':
      case 'X':
        testPowerLoss();
        break;
        
      case 'h':
      case 'H':
        printHelpCommands();
        break;
        
      default:
        if (command > 32) {
          Serial.print("Unknown command: ");
          Serial.println(command);
          Serial.println("Send 'h' for help");
        }
        break;
    }
  }
}

void printCurrentStatus() {
  if (!rtcInitialized) {
    Serial.println("❌ DS3231 not initialized");
    return;
  }
  
  DateTime now = rtc.now();
  
  Serial.println("\n=== DS3231 RTC Status ===");
  Serial.print("Current time: ");
  printDateTime(now);
  Serial.println();
  
  Serial.print("Unix timestamp: ");
  Serial.println(now.unixtime());
  
  Serial.print("Test running for: ");
  Serial.print((millis() - testStartTime) / 1000);
  Serial.println(" seconds");
  
  Serial.print("Total test cycles: ");
  Serial.println(testCount);
  
  Serial.print("Total interrupts: ");
  Serial.println(tickCount);
  
  if (timeDriftSeconds != 0) {
    Serial.print("Time drift: ");
    Serial.print(timeDriftSeconds);
    Serial.println(" seconds");
  }
  
  Serial.println("========================\n");
}

void printInterruptStats() {
  Serial.println("\n=== INTERRUPT STATISTICS ===");
  
  Serial.print("Total ticks received: ");
  Serial.println(tickCount);
  
  unsigned long uptimeSeconds = (millis() - testStartTime) / 1000;
  Serial.print("Expected ticks: ");
  Serial.println(uptimeSeconds);
  
  long tickDifference = (long)tickCount - (long)uptimeSeconds;
  Serial.print("Tick difference: ");
  Serial.print(tickDifference);
  
  if (tickDifference == 0) {
    Serial.println(" ✅ PERFECT");
  } else if (abs(tickDifference) <= 2) {
    Serial.println(" ✅ EXCELLENT (±2)");
  } else if (abs(tickDifference) <= 5) {
    Serial.println(" ⚠️  Acceptable (±5)");
  } else {
    Serial.println(" ❌ PROBLEM");
  }
  
  if (tickCount > 0 && uptimeSeconds > 0) {
    float accuracy = 100.0 * tickCount / uptimeSeconds;
    Serial.print("Interrupt accuracy: ");
    Serial.print(accuracy, 2);
    Serial.println("%");
  }
  
  Serial.print("Last tick at: ");
  Serial.print(lastTickMillis);
  Serial.println(" ms");
  
  unsigned long timeSinceLastTick = millis() - lastTickMillis;
  Serial.print("Time since last tick: ");
  Serial.print(timeSinceLastTick);
  Serial.println(" ms");
  
  if (timeSinceLastTick > 1500) {
    Serial.println("⚠️  WARNING: No tick in last 1.5 seconds!");
  }
  
  Serial.println("============================\n");
}

void printDetailedStatus() {
  Serial.println("\n================================");
  Serial.println("    DETAILED RTC DIAGNOSTICS");
  Serial.println("================================");
  
  if (!rtcInitialized) {
    Serial.println("❌ DS3231 not initialized - no diagnostics available");
    return;
  }
  
  DateTime now = rtc.now();
  
  Serial.print("📅 Date: ");
  Serial.print(now.day());
  Serial.print("/");
  Serial.print(now.month());
  Serial.print("/");
  Serial.println(now.year());
  
  Serial.print("🕐 Time: ");
  Serial.print(now.hour());
  Serial.print(":");
  if (now.minute() < 10) Serial.print("0");
  Serial.print(now.minute());
  Serial.print(":");
  if (now.second() < 10) Serial.print("0");
  Serial.println(now.second());
  
  float currentTemp = rtc.getTemperature();
  Serial.print("🌡️  Temperature: ");
  Serial.print(currentTemp);
  Serial.println("°C");
  
  if (temperatureReadings > 0) {
    float avgTemp = temperatureSum / temperatureReadings;
    Serial.print("   Average temp: ");
    Serial.print(avgTemp);
    Serial.println("°C");
  }
  
  Serial.print("⏱️  Timing accuracy: ");
  if (abs(timeDriftSeconds) <= 1) {
    Serial.println("Excellent (±1s)");
  } else if (abs(timeDriftSeconds) <= 5) {
    Serial.println("Good (±5s)");
  } else if (abs(timeDriftSeconds) <= 30) {
    Serial.println("Fair (±30s)");
  } else {
    Serial.println("Poor (>30s drift)");
  }
  
  Serial.print("🔋 Power status: ");
  if (rtc.lostPower()) {
    Serial.println("⚠️  Lost power recently");
  } else {
    Serial.println("✅ Stable");
  }
  
  Serial.print("📊 Test statistics: ");
  Serial.print(testCount);
  Serial.print(" cycles, ");
  Serial.print((millis() - testStartTime) / 1000);
  Serial.println(" seconds uptime");
  
  Serial.print("⚡ Interrupt status: ");
  Serial.print(tickCount);
  Serial.println(" ticks received");
  
  Serial.println("================================\n");
}

void resetTimeToCompileTime() {
  Serial.println("Resetting DS3231 to compile time...");
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  DateTime newTime = rtc.now();
  Serial.print("Time set to: ");
  printDateTime(newTime);
  Serial.println();
  
  testStartTime = millis();
  timeDriftSeconds = 0;
  lastTimeReading = newTime;
  
  // Reset interrupt counters
  noInterrupts();
  tickCount = 0;
  lastTickMillis = millis();
  interrupts();
  
  Serial.println("✅ Time reset complete (interrupt counters reset)\n");
}

void printTemperatureInfo() {
  float currentTemp = rtc.getTemperature();
  
  Serial.println("\n=== DS3231 Temperature Info ===");
  Serial.print("Current temperature: ");
  Serial.print(currentTemp);
  Serial.println("°C");
  
  if (temperatureReadings > 0) {
    float avgTemp = temperatureSum / temperatureReadings;
    Serial.print("Average temperature: ");
    Serial.print(avgTemp);
    Serial.println("°C");
    Serial.print("Temperature readings: ");
    Serial.println(temperatureReadings);
  }
  
  Serial.println("\nNotes:");
  Serial.println("- DS3231 has built-in temperature compensation");
  Serial.println("- Temperature affects crystal oscillator accuracy");
  Serial.println("- Typical accuracy: ±3°C");
  Serial.println("- Used internally for timing compensation");
  Serial.println("==============================\n");
}

void checkBatteryStatus() {
  Serial.println("\n=== Battery Backup Status ===");
  
  if (rtc.lostPower()) {
    Serial.println("❌ Battery backup failed or not present");
    Serial.println("   - RTC lost power since last set");
    Serial.println("   - Install CR2032 battery for backup");
  } else {
    Serial.println("✅ Battery backup working");
    Serial.println("   - RTC maintained time through power cycles");
  }
  
  Serial.println("\nBattery Backup Tips:");
  Serial.println("- Use CR2032 lithium battery");
  Serial.println("- Battery life: 8-10 years typically");
  Serial.println("- Replace when RTC starts losing time");
  Serial.println("==============================\n");
}

void printDetailedDiagnostics() {
  Serial.println("\n=== ADVANCED DIAGNOSTICS ===");
  
  DateTime now = rtc.now();
  
  Serial.print("Day of week: ");
  Serial.print(now.dayOfTheWeek());
  Serial.print(" (");
  const char* days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  Serial.print(days[now.dayOfTheWeek()]);
  Serial.println(")");
  
  unsigned long unixTime = now.unixtime();
  Serial.print("Unix timestamp: ");
  Serial.println(unixTime);
  
  if (unixTime < 1577836800) {
    Serial.println("⚠️  WARNING: Time appears to be before 2020");
  } else if (unixTime > 2524608000) {
    Serial.println("⚠️  WARNING: Time appears to be after 2050");
  } else {
    Serial.println("✅ Timestamp is in reasonable range");
  }
  
  Serial.println("\nDS3231 Features:");
  Serial.println("- High accuracy (±2ppm, ±1 minute/year)");
  Serial.println("- Temperature compensated crystal oscillator");
  Serial.println("- Battery backup with CR2032");
  Serial.println("- I2C interface (400kHz max)");
  Serial.println("- Built-in temperature sensor");
  Serial.println("- Two programmable alarms");
  Serial.println("- 1Hz square wave output (SQW pin) ⚡");
  
  Serial.println("=============================\n");
}

void testPowerLoss() {
  Serial.println("\n=== POWER LOSS TEST ===");
  Serial.println("This test simulates power loss by reading time before and after");
  Serial.println("you disconnect main power (USB cable)");
  Serial.println();
  
  DateTime beforePowerLoss = rtc.now();
  Serial.print("Time BEFORE power loss: ");
  printDateTime(beforePowerLoss);
  Serial.println();
  
  Serial.println("📋 INSTRUCTIONS:");
  Serial.println("1. Note the time displayed above");
  Serial.println("2. Disconnect USB cable from Arduino for 10+ seconds");
  Serial.println("3. Reconnect USB cable");
  Serial.println("4. Open serial monitor again");
  Serial.println("5. Send 'x' command again to check time after power loss");
  Serial.println();
  
  static DateTime lastRecordedTime;
  static bool hasRecordedTime = false;
  
  if (hasRecordedTime) {
    DateTime afterPowerLoss = rtc.now();
    Serial.println("⚡ POWER LOSS TEST RESULTS:");
    Serial.print("Time BEFORE power loss: ");
    printDateTime(lastRecordedTime);
    Serial.println();
    Serial.print("Time AFTER power loss:  ");
    printDateTime(afterPowerLoss);
    Serial.println();
    
    long timeDifference = afterPowerLoss.unixtime() - lastRecordedTime.unixtime();
    Serial.print("Time elapsed: ");
    Serial.print(timeDifference);
    Serial.println(" seconds");
    
    if (rtc.lostPower()) {
      Serial.println("❌ FAILED: DS3231 detected power loss!");
      Serial.println("   - Check CR2032 battery connection");
      Serial.println("   - Battery may be dead or missing");
    } else {
      Serial.println("✅ PASSED: DS3231 maintained time during power loss!");
      Serial.println("   - Battery backup working correctly");
      Serial.println("   - Time continued advancing normally");
    }
    
    hasRecordedTime = false;
  } else {
    lastRecordedTime = beforePowerLoss;
    hasRecordedTime = true;
    Serial.println("⚠️  Time recorded. Now disconnect power as instructed above.");
  }
  
  Serial.println("===============================\n");
}

void printHelpCommands() {
  Serial.println("\n=== Available Commands ===");
  Serial.println("s - Show current status");
  Serial.println("i - Show interrupt statistics ⚡ NEW");
  Serial.println("r - Reset time to compile time");
  Serial.println("t - Show temperature information");
  Serial.println("d - Detailed diagnostics");
  Serial.println("b - Check battery backup status");
  Serial.println("x - Power loss test");
  Serial.println("h - Show this help");
  Serial.println("==========================\n");
}

void printDateTime(DateTime dt) {
  Serial.print(dt.year());
  Serial.print("/");
  if (dt.month() < 10) Serial.print("0");
  Serial.print(dt.month());
  Serial.print("/");
  if (dt.day() < 10) Serial.print("0");
  Serial.print(dt.day());
  Serial.print(" ");
  if (dt.hour() < 10) Serial.print("0");
  Serial.print(dt.hour());
  Serial.print(":");
  if (dt.minute() < 10) Serial.print("0");
  Serial.print(dt.minute());
  Serial.print(":");
  if (dt.second() < 10) Serial.print("0");
  Serial.print(dt.second());
}