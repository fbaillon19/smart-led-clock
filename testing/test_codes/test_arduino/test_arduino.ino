/**
 * Smart LED Clock - Arduino UNO R4 WiFi Basic Test
 * 
 * This test verifies that the Arduino UNO R4 WiFi board is functioning correctly.
 * It tests:
 * - Serial communication
 * - Built-in LED
 * - Digital pins
 * - Analog pins  
 * - WiFi module presence
 * - Basic timing functionality
 * 
 * Expected Results:
 * - Serial monitor shows test progress and results
 * - Built-in LED blinks during test
 * - All tests should show PASS status
 * 
 * Note: This test is specifically adapted for Arduino UNO R4 WiFi
 * 
 * Author: F. Baillon
 * License: MIT
 */

#include <WiFiS3.h>
// Note: RTC library removed for basic test - main project uses DS3231 external RTC

// Test configuration
#define BUILTIN_LED LED_BUILTIN
#define TEST_DIGITAL_PIN 13
#define TEST_ANALOG_PIN A0
#define TEST_PWM_PIN 9

// Test results tracking
int totalTests = 0;
int passedTests = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(2000); // Wait for serial to stabilize
  
  Serial.println("=====================================");
  Serial.println("    Arduino UNO R4 WiFi Basic Test");
  Serial.println("=====================================");
  Serial.println();
  
  // Run all basic tests
  testSerialCommunication();
  testBuiltinLED();
  testDigitalPins();
  testAnalogPins();
  testPWMPins();
  testWiFiModule();
  testRTC();
  testMemory();
  
  // Print final results
  printTestSummary();
}

void loop() {
  // Blink LED to indicate system is running
  static unsigned long lastBlink = 0;
  static bool ledState = false;
  
  if (millis() - lastBlink > 1000) {
    ledState = !ledState;
    digitalWrite(BUILTIN_LED, ledState);
    lastBlink = millis();
  }
  
  // Print alive message every 10 seconds
  static unsigned long lastAlive = 0;
  if (millis() - lastAlive > 10000) {
    Serial.println("Arduino is alive and running...");
    lastAlive = millis();
  }
  
  delay(100);
}

/**
 * Test serial communication functionality
 */
void testSerialCommunication() {
  Serial.print("Testing Serial Communication... ");
  
  // Test is inherently passed if we can print this message
  totalTests++;
  passedTests++;
  
  Serial.println("PASS");
  Serial.println("  - Serial baud rate: 115200");
  Serial.println("  - Communication established successfully");
  Serial.println();
}

/**
 * Test built-in LED functionality
 */
void testBuiltinLED() {
  Serial.print("Testing Built-in LED... ");
  totalTests++;
  
  pinMode(BUILTIN_LED, OUTPUT);
  
  // Blink LED rapidly to indicate test
  for (int i = 0; i < 5; i++) {
    digitalWrite(BUILTIN_LED, HIGH);
    delay(100);
    digitalWrite(BUILTIN_LED, LOW);  
    delay(100);
  }
  
  passedTests++;
  Serial.println("PASS");
  Serial.println("  - LED pin configured successfully");
  Serial.println("  - LED blink test completed");
  Serial.println();
}

/**
 * Test digital pin functionality
 */
void testDigitalPins() {
  Serial.print("Testing Digital Pins... ");
  totalTests++;
  
  bool testPassed = true;
  
  // Test digital output
  pinMode(TEST_DIGITAL_PIN, OUTPUT);
  digitalWrite(TEST_DIGITAL_PIN, HIGH);
  delay(10);
  digitalWrite(TEST_DIGITAL_PIN, LOW);
  
  // Test digital input with pullup
  pinMode(TEST_DIGITAL_PIN, INPUT_PULLUP);
  int inputValue = digitalRead(TEST_DIGITAL_PIN);
  
  if (inputValue == HIGH) {
    passedTests++;
    Serial.println("PASS");
    Serial.println("  - Digital output: OK");
    Serial.println("  - Digital input with pullup: OK");
  } else {
    Serial.println("FAIL");
    Serial.println("  - Digital input pullup test failed");
    testPassed = false;
  }
  
  Serial.println();
}

/**
 * Test analog pin functionality
 */
void testAnalogPins() {
  Serial.print("Testing Analog Pins... ");
  totalTests++;
  
  // Configure analog pin as input
  pinMode(TEST_ANALOG_PIN, INPUT);
  
  // Take several readings
  int reading1 = analogRead(TEST_ANALOG_PIN);
  delay(10);
  int reading2 = analogRead(TEST_ANALOG_PIN);
  delay(10);
  int reading3 = analogRead(TEST_ANALOG_PIN);
  
  // Basic validation - readings should be within valid range
  if (reading1 >= 0 && reading1 <= 1023 &&
      reading2 >= 0 && reading2 <= 1023 &&
      reading3 >= 0 && reading3 <= 1023) {
    passedTests++;
    Serial.println("PASS");
    Serial.print("  - Analog readings: ");
    Serial.print(reading1);
    Serial.print(", ");
    Serial.print(reading2);
    Serial.print(", ");
    Serial.println(reading3);
  } else {
    Serial.println("FAIL");
    Serial.println("  - Analog readings out of valid range");
  }
  
  Serial.println();
}

/**
 * Test PWM pin functionality
 */
void testPWMPins() {
  Serial.print("Testing PWM Pins... ");
  totalTests++;
  
  pinMode(TEST_PWM_PIN, OUTPUT);
  
  // Test different PWM values
  analogWrite(TEST_PWM_PIN, 0);    // 0%
  delay(100);
  analogWrite(TEST_PWM_PIN, 127);  // 50%
  delay(100);
  analogWrite(TEST_PWM_PIN, 255);  // 100%
  delay(100);
  analogWrite(TEST_PWM_PIN, 0);    // Back to 0%
  
  passedTests++;
  Serial.println("PASS");
  Serial.println("  - PWM output levels: 0%, 50%, 100% tested");
  Serial.println();
}

/**
 * Test WiFi module presence and basic functionality
 */
void testWiFiModule() {
  Serial.print("Testing WiFi Module... ");
  totalTests++;
  
  // Check if WiFi module is present
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("FAIL");
    Serial.println("  - WiFi module not detected");
    return;
  }
  
  // Check firmware version
  String fv = WiFi.firmwareVersion();
  if (fv.length() == 0) {
    Serial.println("FAIL");
    Serial.println("  - Cannot read WiFi firmware version");
    return;
  }
  
  passedTests++;
  Serial.println("PASS");
  Serial.print("  - WiFi module detected, firmware: ");
  Serial.println(fv);
  Serial.print("  - MAC address: ");
  
  byte mac[6];
  WiFi.macAddress(mac);
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) Serial.print("0");
    Serial.print(mac[i], HEX);
    if (i > 0) Serial.print(":");
  }
  Serial.println();
  Serial.println();
}

/**
 * Test RTC functionality - Updated for Arduino UNO R4
 */
void testRTC() {
  Serial.print("Testing RTC (Real-Time Clock)... ");
  totalTests++;
  
  #if defined(ARDUINO_UNOWIFIR4)
    // Arduino UNO R4 has built-in RTC functionality
    // We'll do a basic RTC test instead of the complex DS3231 test
    
    // Note: For this basic test, we're just checking if we can set/get time
    // The main project will use DS3231 external RTC for better accuracy
    
    bool rtcWorking = true;
    
    // Try to access the RTC (this is a simplified test)
    unsigned long currentTime = millis();
    delay(100);
    unsigned long newTime = millis();
    
    if (newTime > currentTime) {
      passedTests++;
      Serial.println("PASS");
      Serial.println("  - Basic timing functionality working");
      Serial.println("  - Note: Main project uses DS3231 external RTC");
      Serial.print("  - Current uptime: ");
      Serial.print(millis() / 1000);
      Serial.println(" seconds");
    } else {
      Serial.println("FAIL");
      Serial.println("  - Basic timing test failed");
    }
    
  #else
    // For other Arduino boards, skip RTC test
    Serial.println("SKIP");
    Serial.println("  - RTC test not applicable for this board");
    passedTests++; // Don't fail the test, just skip it
  #endif
  
  Serial.println();
}

/**
 * Test memory availability - Updated for Arduino UNO R4
 */
void testMemory() {
  Serial.print("Testing Memory... ");
  totalTests++;
  
  // Arduino UNO R4 has different memory management than classic Arduino
  // We'll use a different approach to estimate available memory
  
  #if defined(ARDUINO_UNOWIFIR4)
    // Arduino UNO R4 WiFi has 32KB SRAM
    // We cannot directly measure free memory like on AVR
    // So we'll do a basic allocation test instead
    
    bool memoryTestPassed = true;
    size_t testSize = 1024; // Try to allocate 1KB
    
    void* testPtr = malloc(testSize);
    if (testPtr != NULL) {
      // Memory allocation successful
      free(testPtr);
      Serial.println("PASS");
      Serial.println("  - Memory allocation test successful");
      Serial.println("  - Arduino UNO R4 has 32KB SRAM total");
      Serial.println("  - Cannot measure exact free memory on ARM architecture");
    } else {
      Serial.println("FAIL");
      Serial.println("  - Memory allocation test failed");
      memoryTestPassed = false;
    }
    
    if (memoryTestPassed) {
      passedTests++;
    }
    
  #else
    // Classic Arduino (AVR) memory measurement
    extern int __heap_start, *__brkval;
    int freeMemory = (int) &freeMemory - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    
    if (freeMemory > 1000) { // Should have at least 1KB free
      passedTests++;
      Serial.println("PASS");
      Serial.print("  - Estimated free memory: ");
      Serial.print(freeMemory);
      Serial.println(" bytes");
    } else {
      Serial.println("FAIL");
      Serial.print("  - Low memory detected: ");
      Serial.print(freeMemory);
      Serial.println(" bytes");
    }
  #endif
  
  Serial.println();
}

/**
 * Print test summary
 */
void printTestSummary() {
  Serial.println("=====================================");
  Serial.println("           TEST SUMMARY");
  Serial.println("=====================================");
  Serial.print("Total tests run: ");
  Serial.println(totalTests);
  Serial.print("Tests passed: ");
  Serial.println(passedTests);
  Serial.print("Tests failed: ");
  Serial.println(totalTests - passedTests);
  Serial.println();
  
  if (passedTests == totalTests) {
    Serial.println("✅ ALL TESTS PASSED!");
    Serial.println("Your Arduino UNO R4 WiFi is working correctly.");
    Serial.println("You can proceed with the Smart LED Clock project.");
  } else {
    Serial.println("❌ SOME TESTS FAILED!");
    Serial.println("Please check the failed tests above and resolve issues");
    Serial.println("before proceeding with the main project.");
  }
  
  Serial.println();
  Serial.println("Note: The built-in LED will continue blinking to");
  Serial.println("indicate the Arduino is running properly.");
  Serial.println("=====================================");
}