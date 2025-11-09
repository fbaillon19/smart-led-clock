/**
 * Smart LED Clock - Button Test
 * 
 * This test verifies that push button is working correctly:
 * - Button wiring and connections
 * - Pull-up resistor functionality
 * - Debouncing and timing
 * - Single click, double click, and long press detection
 * - Button responsiveness and reliability
 * 
 * Expected Results:
 * - Button should respond to all press types
 * - Serial monitor shows button events
 * - No false triggers or missed presses
 * - Proper debouncing behavior
 * 
 * Author: F. Baillon
 * License: MIT
 */

#include <OneButton.h>

// Pin definitions (match your actual wiring)
#define PIN_BUTTON    13     // Button on digital pin 13

// Button objects
OneButton button;

// Test tracking
unsigned long testStartTime;
int buttonClicks = 0;
int buttonDoubleClicks = 0;
int buttonLongPresses = 0;

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
  button.setup(PIN_BUTTON, INPUT_PULLUP, true);
  
  // Configure button timing
  configureButton();
  
  // Attach event handlers
  attachButtonEvents();
  
  testStartTime = millis();
  
  Serial.println("Button test initialized successfully!");
  Serial.println();
  Serial.println("Test Instructions:");
  Serial.println("- Press Button (pin 2) for single click test");
  Serial.println("- Double-click Button for double click test");
  Serial.println("- Hold Button for 1+ second for long press test");
  Serial.println();
  Serial.println("Press button to test functionality...");
  Serial.println("Send 's' for status, 'r' to reset counters, 'h' for help");
  Serial.println();
  
  printCurrentStatus();
}

void loop() {
  // Process button events
  button.tick();
  
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
 * Configure timing parameters for button
 */
void configureButton() {
  button.setDebounceTicks(50);      // 50ms debounce
  button.setClickTicks(400);        // 400ms max for single click
  button.setPressTicks(1000);       // 1000ms minimum for long press
  
  Serial.println("Button timing configured:");
  Serial.println("- Debounce: 50ms");
  Serial.println("- Click timeout: 400ms");
  Serial.println("- Long press: 1000ms");
}

/**
 * Attach event handlers for button
 */
void attachButtonEvents() {
  button.attachClick(onButtonClick);
  button.attachDoubleClick(onButtonDoubleClick);
  button.attachLongPressStart(onButtonLongPressStart);
  button.attachLongPressStop(onButtonLongPressStop);
  button.attachDuringLongPress(onButtonDuringLongPress);
}


// ==========================================
// BUTTON 1 EVENT HANDLERS
// ==========================================
void onButtonClick() {
  buttonClicks++;
  Serial.print("🔸 Button CLICK #");
  Serial.println(buttonClicks);
}

void onButtonDoubleClick() {
  buttonDoubleClicks++;
  Serial.print("🔸🔸 Button DOUBLE-CLICK #");
  Serial.println(buttonDoubleClicks);
}

void onButtonLongPressStart() {
  Serial.println("🔸⏳ Button LONG PRESS started...");
}

void onButtonLongPressStop() {
  buttonLongPresses++;
  Serial.print("🔸⏹️ Button LONG PRESS stopped #");
  Serial.println(buttonLongPresses);
}

void onButtonDuringLongPress() {
  static unsigned long lastDuringPrint = 0;
  if (millis() - lastDuringPrint >= 500) { // Print every 500ms during long press
    Serial.println("🔸⏳ Button LONG PRESS continuing...");
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
  static bool lastButtonState = HIGH;
  
  bool currentButtonState = digitalRead(PIN_BUTTON);
  
  // Report state changes
  if (currentButtonState != lastButtonState) {
    Serial.print("Button raw state: ");
    Serial.println(currentButtonState == LOW ? "PRESSED" : "RELEASED");
    lastButtonState = currentButtonState;
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
  
  Serial.println("Button Events:");
  Serial.print("  Single clicks: ");
  Serial.println(buttonClicks);
  Serial.print("  Double clicks: ");
  Serial.println(buttonDoubleClicks);
  Serial.print("  Long presses: ");
  Serial.println(buttonLongPresses);
  
  int totalEvents = buttonClicks + buttonDoubleClicks + buttonLongPresses;
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
  Serial.print(buttonClicks + buttonDoubleClicks + buttonLongPresses);
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
  buttonClicks = 0;
  buttonDoubleClicks = 0;
  buttonLongPresses = 0;
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
  Serial.println("Press and hold button for 3 seconds...");
  
  unsigned long startTime = millis();
  bool testPassed = false;
  
  while (millis() - startTime < 5000) { // 5 second timeout
    bool buttonPressed = (digitalRead(PIN_BUTTON) == LOW);
    
    if (buttonPressed) {
      unsigned long pressedStart = millis();
      
      // Check if stay pressed for 3 seconds
      while (millis() - pressedStart < 3000) {
        if (digitalRead(PIN_BUTTON) != LOW) {
          break;      // Button was released
        }
        delay(50);
      }
      
      if (millis() - pressedStart >= 3000) {
        testPassed = true;
        break;
      }
    }
    
    delay(100);
  }
  
  if (testPassed) {
    Serial.println("✅ AUTOMATIC TEST PASSED!");
    Serial.println("   Button is properly connected and responding");
  } else {
    Serial.println("❌ AUTOMATIC TEST FAILED!");
    Serial.println("   Check button wiring:");
    Serial.println("   - Button → Pin 13 and GND");
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
  Serial.print("- Button Pin: ");
  Serial.println(PIN_BUTTON);
  Serial.println("- Pull-up: Internal (INPUT_PULLUP)");
  Serial.println("- Active: LOW (pressed = LOW)");
  
  Serial.println("\nTiming Configuration:");
  Serial.println("- Debounce time: 50ms");
  Serial.println("- Click timeout: 400ms");
  Serial.println("- Long press time: 1000ms");
  
  Serial.println("\nExpected Wiring:");
  Serial.println("Button: One leg to Pin 13, other leg to GND");
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
  Serial.println("🔸 = Button events");
  Serial.println("==========================\n");
}