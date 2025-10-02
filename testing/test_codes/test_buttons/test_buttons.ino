/**
 * Smart LED Clock - Button Test
 * 
 * This test verifies that both push buttons are working correctly:
 * - Button wiring and connections
 * - Pull-up resistor functionality
 * - Debouncing and timing
 * - Single click, double click, and long press detection
 * - Button responsiveness and reliability
 * 
 * Expected Results:
 * - Buttons should respond to all press types
 * - Serial monitor shows button events
 * - No false triggers or missed presses
 * - Proper debouncing behavior
 * 
 * Author: F. Baillon
 * License: MIT
 */

#include <OneButton.h>

// Pin definitions (match your actual wiring)
#define PIN_BUTTON_1    2     // Button 1 on digital pin 2
#define PIN_BUTTON_2    3     // Button 2 on digital pin 3

// Button objects
OneButton button1;
OneButton button2;

// Test tracking
unsigned long testStartTime;
int button1Clicks = 0;
int button1DoubleClicks = 0;
int button1LongPresses = 0;
int button2Clicks = 0;
int button2DoubleClicks = 0;
int button2LongPresses = 0;

// Timing for continuous monitoring
unsigned long lastStatusReport = 0;
const unsigned long STATUS_INTERVAL = 5000; // Report every 5 seconds

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=====================================");
  Serial.println("   Smart LED Clock - Button Test");
  Serial.println("=====================================");
  Serial.println();
  
  // Initialize buttons with internal pull-up
  button1.setup(PIN_BUTTON_1, INPUT_PULLUP, true);
  button2.setup(PIN_BUTTON_2, INPUT_PULLUP, true);
  
  // Configure button timing
  configureBothButtons();
  
  // Attach event handlers
  attachButton1Events();
  attachButton2Events();
  
  testStartTime = millis();
  
  Serial.println("Button test initialized successfully!");
  Serial.println();
  Serial.println("Test Instructions:");
  Serial.println("- Press Button 1 (pin 2) for single click test");
  Serial.println("- Double-click Button 1 for double click test");
  Serial.println("- Hold Button 1 for 1+ second for long press test");
  Serial.println("- Press Button 2 (pin 3) for single click test");
  Serial.println("- Double-click Button 2 for double click test");
  Serial.println("- Hold Button 2 for 1+ second for long press test");
  Serial.println();
  Serial.println("Press buttons to test functionality...");
  Serial.println("Send 's' for status, 'r' to reset counters, 'h' for help");
  Serial.println();
  
  printCurrentStatus();
}

void loop() {
  // Process button events
  button1.tick();
  button2.tick();
  
  // Handle serial commands
  handleSerialCommands();
  
  // Print status report periodically
  if (millis() - lastStatusReport >= STATUS_INTERVAL) {
    printPeriodicStatus();
    lastStatusReport = millis();
  }
  
  // Monitor button states continuously
  static unsigned long lastStateCheck = 0;
  if (millis() - lastStateCheck >= 100) { // Check every 100ms
    checkButtonStates();
    lastStateCheck = millis();
  }
  
  delay(10); // Small delay for stability
}

/**
 * Configure timing parameters for both buttons
 */
void configureBothButtons() {
  // Button 1 configuration
  button1.setDebounceTicks(50);      // 50ms debounce
  button1.setClickTicks(400);        // 400ms max for single click
  button1.setPressTicks(1000);       // 1000ms minimum for long press
  
  // Button 2 configuration  
  button2.setDebounceTicks(50);      // 50ms debounce
  button2.setClickTicks(400);        // 400ms max for single click
  button2.setPressTicks(1000);       // 1000ms minimum for long press
  
  Serial.println("Button timing configured:");
  Serial.println("- Debounce: 50ms");
  Serial.println("- Click timeout: 400ms");
  Serial.println("- Long press: 1000ms");
}

/**
 * Attach event handlers for button 1
 */
void attachButton1Events() {
  button1.attachClick(onButton1Click);
  button1.attachDoubleClick(onButton1DoubleClick);
  button1.attachLongPressStart(onButton1LongPressStart);
  button1.attachLongPressStop(onButton1LongPressStop);
  button1.attachDuringLongPress(onButton1DuringLongPress);
}

/**
 * Attach event handlers for button 2
 */
void attachButton2Events() {
  button2.attachClick(onButton2Click);
  button2.attachDoubleClick(onButton2DoubleClick);
  button2.attachLongPressStart(onButton2LongPressStart);
  button2.attachLongPressStop(onButton2LongPressStop);
  button2.attachDuringLongPress(onButton2DuringLongPress);
}

// ==========================================
// BUTTON 1 EVENT HANDLERS
// ==========================================

void onButton1Click() {
  button1Clicks++;
  Serial.print("🔸 Button 1 CLICK #");
  Serial.println(button1Clicks);
}

void onButton1DoubleClick() {
  button1DoubleClicks++;
  Serial.print("🔸🔸 Button 1 DOUBLE-CLICK #");
  Serial.println(button1DoubleClicks);
}

void onButton1LongPressStart() {
  Serial.println("🔸⏳ Button 1 LONG PRESS started...");
}

void onButton1LongPressStop() {
  button1LongPresses++;
  Serial.print("🔸⏹️ Button 1 LONG PRESS stopped #");
  Serial.println(button1LongPresses);
}

void onButton1DuringLongPress() {
  static unsigned long lastDuringPrint = 0;
  if (millis() - lastDuringPrint >= 500) { // Print every 500ms during long press
    Serial.println("🔸⏳ Button 1 LONG PRESS continuing...");
    lastDuringPrint = millis();
  }
}

// ==========================================
// BUTTON 2 EVENT HANDLERS
// ==========================================

void onButton2Click() {
  button2Clicks++;
  Serial.print("🔹 Button 2 CLICK #");
  Serial.println(button2Clicks);
}

void onButton2DoubleClick() {
  button2DoubleClicks++;
  Serial.print("🔹🔹 Button 2 DOUBLE-CLICK #");
  Serial.println(button2DoubleClicks);
}

void onButton2LongPressStart() {
  Serial.println("🔹⏳ Button 2 LONG PRESS started...");
}

void onButton2LongPressStop() {
  button2LongPresses++;
  Serial.print("🔹⏹️ Button 2 LONG PRESS stopped #");
  Serial.println(button2LongPresses);
}

void onButton2DuringLongPress() {
  static unsigned long lastDuringPrint = 0;
  if (millis() - lastDuringPrint >= 500) { // Print every 500ms during long press
    Serial.println("🔹⏳ Button 2 LONG PRESS continuing...");
    lastDuringPrint = millis();
  }
}

// ==========================================
// STATUS AND MONITORING
// ==========================================

/**
 * Check and report raw button states
 */
void checkButtonStates() {
  static bool lastButton1State = HIGH;
  static bool lastButton2State = HIGH;
  
  bool currentButton1State = digitalRead(PIN_BUTTON_1);
  bool currentButton2State = digitalRead(PIN_BUTTON_2);
  
  // Report state changes
  if (currentButton1State != lastButton1State) {
    Serial.print("Button 1 raw state: ");
    Serial.println(currentButton1State == LOW ? "PRESSED" : "RELEASED");
    lastButton1State = currentButton1State;
  }
  
  if (currentButton2State != lastButton2State) {
    Serial.print("Button 2 raw state: ");
    Serial.println(currentButton2State == LOW ? "PRESSED" : "RELEASED");
    lastButton2State = currentButton2State;
  }
}

/**
 * Print current test status
 */
void printCurrentStatus() {
  Serial.println("=== Button Test Status ===");
  Serial.print("Test running for: ");
  Serial.print((millis() - testStartTime) / 1000);
  Serial.println(" seconds");
  Serial.println();
  
  Serial.println("Button 1 Events:");
  Serial.print("  Single clicks: ");
  Serial.println(button1Clicks);
  Serial.print("  Double clicks: ");
  Serial.println(button1DoubleClicks);
  Serial.print("  Long presses: ");
  Serial.println(button1LongPresses);
  
  Serial.println("Button 2 Events:");
  Serial.print("  Single clicks: ");
  Serial.println(button2Clicks);
  Serial.print("  Double clicks: ");
  Serial.println(button2DoubleClicks);
  Serial.print("  Long presses: ");
  Serial.println(button2LongPresses);
  
  int totalEvents = button1Clicks + button1DoubleClicks + button1LongPresses +
                   button2Clicks + button2DoubleClicks + button2LongPresses;
  Serial.print("Total events: ");
  Serial.println(totalEvents);
  Serial.println("========================");
  Serial.println();
}

/**
 * Print periodic status update
 */
void printPeriodicStatus() {
  Serial.print("📊 Status update - Uptime: ");
  Serial.print((millis() - testStartTime) / 1000);
  Serial.print("s | Events: B1(");
  Serial.print(button1Clicks + button1DoubleClicks + button1LongPresses);
  Serial.print(") B2(");
  Serial.print(button2Clicks + button2DoubleClicks + button2LongPresses);
  Serial.println(")");
}

/**
 * Handle serial commands for interactive testing
 */
void handleSerialCommands() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    
    switch (command) {
      case 's':
      case 'S':
        printCurrentStatus();
        break;
        
      case 'r':
      case 'R':
        resetCounters();
        break;
        
      case 't':
      case 'T':
        runAutomaticTest();
        break;
        
      case 'c':
      case 'C':
        printButtonConfiguration();
        break;
        
      case 'h':
      case 'H':
        printHelpCommands();
        break;
        
      default:
        if (command > 32) { // Printable character
          Serial.print("Unknown command: ");
          Serial.println(command);
          Serial.println("Send 'h' for help");
        }
        break;
    }
  }
}

/**
 * Reset all event counters
 */
void resetCounters() {
  button1Clicks = 0;
  button1DoubleClicks = 0;
  button1LongPresses = 0;
  button2Clicks = 0;
  button2DoubleClicks = 0;
  button2LongPresses = 0;
  testStartTime = millis();
  
  Serial.println("✅ All counters reset to zero");
  Serial.println("✅ Test timer reset");
  Serial.println();
}

/**
 * Run automatic button functionality test
 */
void runAutomaticTest() {
  Serial.println("\n=== AUTOMATIC BUTTON TEST ===");
  Serial.println("This test will check button wiring and basic functionality");
  Serial.println("Press and hold both buttons simultaneously for 3 seconds...");
  
  unsigned long startTime = millis();
  bool testPassed = false;
  
  while (millis() - startTime < 5000) { // 5 second timeout
    bool button1Pressed = (digitalRead(PIN_BUTTON_1) == LOW);
    bool button2Pressed = (digitalRead(PIN_BUTTON_2) == LOW);
    
    if (button1Pressed && button2Pressed) {
      unsigned long bothPressedStart = millis();
      
      // Check if both stay pressed for 3 seconds
      while (millis() - bothPressedStart < 3000) {
        if (digitalRead(PIN_BUTTON_1) != LOW || digitalRead(PIN_BUTTON_2) != LOW) {
          break; // One button was released
        }
        delay(50);
      }
      
      if (millis() - bothPressedStart >= 3000) {
        testPassed = true;
        break;
      }
    }
    
    delay(100);
  }
  
  if (testPassed) {
    Serial.println("✅ AUTOMATIC TEST PASSED!");
    Serial.println("   Both buttons are properly connected and responding");
  } else {
    Serial.println("❌ AUTOMATIC TEST FAILED!");
    Serial.println("   Check button wiring:");
    Serial.println("   - Button 1 → Pin 2 and GND");
    Serial.println("   - Button 2 → Pin 3 and GND");
    Serial.println("   - Ensure buttons make good contact");
  }
  Serial.println("==============================\n");
}

/**
 * Print button configuration details
 */
void printButtonConfiguration() {
  Serial.println("\n=== Button Configuration ===");
  Serial.println("Hardware Setup:");
  Serial.print("- Button 1 Pin: ");
  Serial.println(PIN_BUTTON_1);
  Serial.print("- Button 2 Pin: ");
  Serial.println(PIN_BUTTON_2);
  Serial.println("- Pull-up: Internal (INPUT_PULLUP)");
  Serial.println("- Active: LOW (pressed = LOW)");
  
  Serial.println("\nTiming Configuration:");
  Serial.println("- Debounce time: 50ms");
  Serial.println("- Click timeout: 400ms");
  Serial.println("- Long press time: 1000ms");
  
  Serial.println("\nExpected Wiring:");
  Serial.println("Button 1: One leg to Pin 2, other leg to GND");
  Serial.println("Button 2: One leg to Pin 3, other leg to GND");
  Serial.println("(No external resistors needed - using internal pull-up)");
  Serial.println("============================\n");
}

/**
 * Print available commands
 */
void printHelpCommands() {
  Serial.println("\n=== Available Commands ===");
  Serial.println("s - Show current status and counters");
  Serial.println("r - Reset all counters and timer");
  Serial.println("t - Run automatic button test");
  Serial.println("c - Show button configuration");
  Serial.println("h - Show this help");
  Serial.println();
  Serial.println("Button Test Events:");
  Serial.println("🔸 = Button 1 events");
  Serial.println("🔹 = Button 2 events");
  Serial.println("==========================\n");
}