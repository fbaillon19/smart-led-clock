/**
 * Smart LED Clock - LCD Display Test
 * 
 * This test verifies that the 20x4 I2C LCD display is working correctly.
 * It tests:
 * - I2C communication
 * - Display initialization
 * - Text display on all lines
 * - Custom character creation
 * - Backlight control
 * - Cursor positioning
 * - Anti-flickering display
 * 
 * ⚠️  POWER REQUIREMENTS:
 * - LCD with backlight can draw 80-150mA
 * - USB power may be insufficient for stable operation
 * - RECOMMENDED: Use external 5V power supply (500mA+)
 * - Arduino Vin pin: 7-12V DC adapter
 * - OR use 5V/1A USB power adapter (not computer USB)
 * 
 * Expected Results:
 * - LCD should display test messages clearly
 * - All 4 lines should show different content
 * - Custom degree symbol should appear
 * - Display should cycle through different test screens
 * - Backlight should turn on/off without flickering
 * - No random characters or display resets
 * 
 * Author: F. Baillon
 * License: MIT
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD Configuration
#define LCD_I2C_ADDRESS 0x27
#define LCD_COLUMNS     20
#define LCD_ROWS        4

// Create LCD object
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// Custom character for degree symbol
byte degreeChar[8] = {
  0b01100,
  0b10010,
  0b10010,
  0b01100,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

// Test state
int currentTest = 0;
unsigned long lastTestChange = 0;
const unsigned long TEST_INTERVAL = 4000;
bool testPaused = false;
int animationStep = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=====================================");
  Serial.println("      LCD Display Test Starting");
  Serial.println("=====================================");
  Serial.println();
  Serial.println("⚠️  POWER WARNING:");
  Serial.println("LCD backlight draws 80-150mA current");
  Serial.println("Computer USB may provide insufficient power");
  Serial.println("RECOMMENDED: Use external 5V power supply");
  Serial.println("  - 5V/1A USB adapter OR");
  Serial.println("  - 7-12V DC adapter to Vin pin");
  Serial.println();
  Serial.println("If you see random characters, display resets,");
  Serial.println("or erratic behavior, switch to external power!");
  Serial.println("=====================================");
  Serial.println();
  
  Wire.begin();
  
  scanI2CDevices();
  
  Serial.print("Initializing LCD at address 0x");
  Serial.print(LCD_I2C_ADDRESS, HEX);
  Serial.println("...");
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  lcd.createChar(0, degreeChar);
  
  Serial.println("LCD initialized successfully!");
  Serial.println("Watch the LCD display for test patterns...");
  Serial.println("Send commands: p=pause, n=next, b=backlight, c=clear, h=help");
  Serial.println();
  
  lastTestChange = millis();
  printTestInfo();
}

void loop() {
  handleSerialCommands();
  
  if (!testPaused) {
    if (millis() - lastTestChange >= TEST_INTERVAL) {
      currentTest = (currentTest + 1) % 14;
      lastTestChange = millis();
      printTestInfo();
      animationStep = 0;
    }
    
    runCurrentTest();
  }
  
  delay(50);
}

void scanI2CDevices() {
  Serial.println("Scanning for I2C devices...");
  
  byte error, address;
  int deviceCount = 0;
  
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      deviceCount++;
    }
  }
  
  if (deviceCount == 0) {
    Serial.println("No I2C devices found!");
    Serial.println("Check your wiring:");
    Serial.println("  SDA -> A4 (or SDA pin)");
    Serial.println("  SCL -> A5 (or SCL pin)");
    Serial.println("  VCC -> 5V");
    Serial.println("  GND -> GND");
  } else {
    Serial.print("Found ");
    Serial.print(deviceCount);
    Serial.println(" I2C device(s)");
  }
  Serial.println();
}

void printTestInfo() {
  Serial.print(">>> Test ");
  Serial.print(currentTest + 1);
  Serial.print("/14: ");
  
  switch (currentTest) {
    case 0: Serial.println("Basic Display Test"); break;
    case 1: Serial.println("All Lines Test"); break;
    case 2: Serial.println("Custom Characters Test"); break;
    case 3: Serial.println("Scrolling Text Test"); break;
    case 4: Serial.println("Cursor Positioning Test"); break;
    case 5: Serial.println("Numbers and Time Test"); break;
    case 6: Serial.println("Special Characters Test"); break;
    case 7: Serial.println("Temperature Display Simulation"); break;
    case 8: Serial.println("Contrast and Visibility Test"); break;
    case 9: Serial.println("Line Clearing Test"); break;
    case 10: Serial.println("Text Alignment Test"); break;
    case 11: Serial.println("Long Text Display Test"); break;
    case 12: Serial.println("Backlight Control Test"); break;
    case 13: Serial.println("Stable Display Test (Anti-flicker)"); break;
  }
}

void runCurrentTest() {
  switch (currentTest) {
    case 0: testBasicDisplay(); break;
    case 1: testAllLines(); break;
    case 2: testCustomCharacters(); break;
    case 3: testScrolling(); break;
    case 4: testCursorPositioning(); break;
    case 5: testNumbers(); break;
    case 6: testSpecialChars(); break;
    case 7: testTemperatureDisplay(); break;
    case 8: testContrast(); break;
    case 9: testLineClearing(); break;
    case 10: testTextAlignment(); break;
    case 11: testLongText(); break;
    case 12: testBacklightControl(); break;
    case 13: testStableDisplay(); break;
  }
}

void testBasicDisplay() {
  static bool contentSet = false;
  
  if (millis() - lastTestChange < 100) {
    contentSet = false;
  }
  
  if (!contentSet) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Smart LED Clock");
    lcd.setCursor(0, 1);
    lcd.print("LCD Test - Basic");
    lcd.setCursor(0, 2);
    lcd.print("20x4 I2C Display");
    lcd.setCursor(0, 3);
    lcd.print("Test 1/14 - OK!");
    contentSet = true;
  }
}

void testAllLines() {
  static bool contentSet = false;
  
  if (millis() - lastTestChange < 100) {
    contentSet = false;
  }
  
  if (!contentSet) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Line 0: 1234567890");
    lcd.setCursor(0, 1);
    lcd.print("Line 1: ABCDEFGHIJ");
    lcd.setCursor(0, 2);
    lcd.print("Line 2: !@#$%^&*()");
    lcd.setCursor(0, 3);
    lcd.print("Line 3: Test 2/14");
    contentSet = true;
  }
}

void testCustomCharacters() {
  static bool contentSet = false;
  
  if (millis() - lastTestChange < 100) {
    contentSet = false;
  }
  
  if (!contentSet) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Custom Characters");
    lcd.setCursor(0, 1);
    lcd.print("Temperature: 23.5");
    lcd.write(byte(0));
    lcd.print("C");
    lcd.setCursor(0, 2);
    lcd.print("Humidity: 65%");
    lcd.setCursor(0, 3);
    lcd.print("Test 3/14 - Custom");
    contentSet = true;
  }
}

void testScrolling() {
  static int scrollPos = 0;
  static unsigned long lastScrollUpdate = 0;
  const char* longText = "This is a very long text that will scroll across the display to test horizontal scrolling functionality perfectly";
  int textLen = strlen(longText);
  
  if (millis() - lastTestChange < 100) {
    scrollPos = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scrolling Test");
    lcd.setCursor(0, 1);
    lcd.print("Test 4/14");
    lcd.setCursor(0, 3);
    lcd.print("Watch line 2 scroll");
  }
  
  if (millis() - lastScrollUpdate >= 300) {
    lcd.setCursor(0, 2);
    lcd.print("                    ");
    lcd.setCursor(0, 2);
    
    for (int i = 0; i < LCD_COLUMNS && (scrollPos + i) < textLen; i++) {
      lcd.print(longText[scrollPos + i]);
    }
    
    scrollPos++;
    if (scrollPos >= textLen) {
      scrollPos = 0;
    }
    
    lastScrollUpdate = millis();
  }
}

void testCursorPositioning() {
  static bool initialized = false;
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastTestChange < 100) {
    initialized = false;
  }
  
  if (!initialized) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cursor Position Test");
    lcd.setCursor(0, 3);
    lcd.print("Test 5/14");
    initialized = true;
    lastUpdate = millis();
  }
  
  if (millis() - lastUpdate >= 200) {
    int seconds = ((millis() - lastTestChange) / 1000) % 4;
    
    lcd.setCursor(0, 1);
    for (int i = 0; i < LCD_COLUMNS; i++) {
      if (i == seconds * 5) {
        lcd.print("*");
      } else {
        lcd.print("-");
      }
    }
    
    lcd.setCursor(0, 2);
    for (int i = 0; i < LCD_COLUMNS; i++) {
      if ((i + seconds * 3) % 8 == 0) {
        lcd.print("|");
      } else {
        lcd.print(" ");
      }
    }
    
    lastUpdate = millis();
  }
}

void testNumbers() {
  static bool headerSet = false;
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastTestChange < 100) {
    headerSet = false;
  }
  
  if (!headerSet) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Numbers Test");
    lcd.setCursor(0, 3);
    lcd.print("Test 6/14 - Numbers");
    headerSet = true;
  }
  
  if (millis() - lastUpdate >= 1000) {
    unsigned long seconds = (millis() - lastTestChange) / 1000;
    int hours = (seconds / 3600) % 24;
    int mins = (seconds / 60) % 60;
    int secs = seconds % 60;
    
    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    if (hours < 10) lcd.print("0");
    lcd.print(hours);
    lcd.print(":");
    if (mins < 10) lcd.print("0");
    lcd.print(mins);
    lcd.print(":");
    if (secs < 10) lcd.print("0");
    lcd.print(secs);
    lcd.print("      ");
    
    lcd.setCursor(0, 2);
    lcd.print("Counter: ");
    lcd.print(seconds);
    lcd.print("        ");
    
    lastUpdate = millis();
  }
}

void testSpecialChars() {
  static bool contentSet = false;
  
  if (millis() - lastTestChange < 100) {
    contentSet = false;
  }
  
  if (!contentSet) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Special Characters");
    lcd.setCursor(0, 1);
    lcd.print("Symbols: []{}()<>");
    lcd.setCursor(0, 2);
    lcd.print("Math: +-*/=<>");
    lcd.setCursor(0, 3);
    lcd.print("Test 7/14 - Symbols");
    contentSet = true;
  }
}

void testTemperatureDisplay() {
  static bool displaySet = false;
  
  if (millis() - lastTestChange < 100) {
    displaySet = false;
  }
  
  if (!displaySet) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("INDOOR  : 22.3");
    lcd.write(byte(0));
    lcd.print("C");
    
    lcd.setCursor(0, 1);
    lcd.print("HUMIDITY: 58%");
    
    lcd.setCursor(0, 2);
    lcd.print("OUTDOOR : 18.7");
    lcd.write(byte(0));
    lcd.print("C");
    
    lcd.setCursor(0, 3);
    lcd.print("Test 8/14 - Temp");
    
    displaySet = true;
  }
}

void testContrast() {
  static bool contentSet = false;
  
  if (millis() - lastTestChange < 100) {
    contentSet = false;
  }
  
  if (!contentSet) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Contrast Test");
    lcd.setCursor(0, 1);
    lcd.print("Check readability:");
    lcd.setCursor(0, 2);
    lcd.print("ABCDEFGHIJKLMNOPQRST");
    lcd.setCursor(0, 3);
    lcd.print("Test 9/14 - Contrast");
    contentSet = true;
  }
}

void testLineClearing() {
  static unsigned long lastClear = 0;
  static int clearLine = 0;
  static bool initialized = false;
  
  if (millis() - lastTestChange < 100) {
    initialized = false;
    clearLine = 0;
  }
  
  if (!initialized) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Line 0: Initial");
    lcd.setCursor(0, 1);
    lcd.print("Line 1: Initial");
    lcd.setCursor(0, 2);
    lcd.print("Line 2: Initial");
    lcd.setCursor(0, 3);
    lcd.print("Line 3: Test 10/14");
    lastClear = millis();
    initialized = true;
  }
  
  if (millis() - lastClear >= 800) {
    if (clearLine < 3) {
      lcd.setCursor(0, clearLine);
      lcd.print("                    ");
      
      lcd.setCursor(0, clearLine);
      lcd.print("Line ");
      lcd.print(clearLine);
      lcd.print(": Cleared!");
      
      clearLine++;
      if (clearLine >= 3) clearLine = 0;
    }
    lastClear = millis();
  }
}

void testTextAlignment() {
  static bool contentSet = false;
  
  if (millis() - lastTestChange < 100) {
    contentSet = false;
  }
  
  if (!contentSet) {
    lcd.clear();
    
    lcd.setCursor(0, 0);
    lcd.print("Left aligned");
    
    String centerText = "Centered";
    int centerPos = (20 - centerText.length()) / 2;
    lcd.setCursor(centerPos, 1);
    lcd.print(centerText);
    
    String rightText = "Right";
    int rightPos = 20 - rightText.length();
    lcd.setCursor(rightPos, 2);
    lcd.print(rightText);
    
    lcd.setCursor(0, 3);
    lcd.print("Test 11/14 - Align");
    
    contentSet = true;
  }
}

void testLongText() {
  static bool contentSet = false;
  
  if (millis() - lastTestChange < 100) {
    contentSet = false;
  }
  
  if (!contentSet) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Long Text Test");
    
    lcd.setCursor(0, 1);
    lcd.print("12345678901234567890");
    
    lcd.setCursor(0, 2);
    lcd.print("This text is definitely longer than 20 characters and will be cut");
    
    lcd.setCursor(0, 3);
    lcd.print("Test 12/14 - Long");
    
    contentSet = true;
  }
}

void testBacklightControl() {
  static unsigned long lastBacklightChange = 0;
  static bool backlightState = true;
  static bool initialized = false;
  
  if (millis() - lastTestChange < 100) {
    initialized = false;
    backlightState = true;
    lcd.backlight();
  }
  
  if (!initialized) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Backlight Test");
    lcd.setCursor(0, 1);
    lcd.print("Watch the backlight");
    lcd.setCursor(0, 3);
    lcd.print("Test 13/14");
    lastBacklightChange = millis();
    initialized = true;
  }
  
  if (millis() - lastBacklightChange >= 1500) {
    backlightState = !backlightState;
    
    if (backlightState) {
      lcd.backlight();
    } else {
      lcd.noBacklight();
    }
    
    lcd.setCursor(0, 2);
    lcd.print("Status: ");
    lcd.print(backlightState ? "ON " : "OFF");
    lcd.print("           ");
    
    lastBacklightChange = millis();
  }
}

void testStableDisplay() {
  static bool initialized = false;
  static unsigned long lastIndicatorUpdate = 0;
  static bool indicatorState = false;
  
  if (millis() - lastTestChange < 100) {
    initialized = false;
  }
  
  if (!initialized) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Stable Display Test");
    lcd.setCursor(0, 1);
    lcd.print("No flickering should");
    lcd.setCursor(0, 2);
    lcd.print("be visible anywhere");
    lcd.setCursor(0, 3);
    lcd.print("Test 14/14 - Final ");
    initialized = true;
  }
  
  if (millis() - lastIndicatorUpdate >= 2000) {
    lcd.setCursor(19, 3);
    lcd.print(indicatorState ? "*" : " ");
    indicatorState = !indicatorState;
    lastIndicatorUpdate = millis();
  }
}

void handleSerialCommands() {
  if (Serial.available() > 0) {
    String command = Serial.readString();
    command.trim();
    command.toLowerCase();
    
    if (command == "p") {
      testPaused = !testPaused;
      Serial.println(testPaused ? "Tests PAUSED" : "Tests RESUMED");
    } else if (command == "n") {
      currentTest = (currentTest + 1) % 14;
      lastTestChange = millis();
      animationStep = 0;
      printTestInfo();
    } else if (command == "b") {
      static bool backlightOn = true;
      backlightOn = !backlightOn;
      if (backlightOn) {
        lcd.backlight();
        Serial.println("Backlight ON");
      } else {
        lcd.noBacklight();
        Serial.println("Backlight OFF");
      }
    } else if (command == "c") {
      lcd.clear();
      Serial.println("Display cleared");
    } else if (command == "help" || command == "h") {
      printHelp();
    } else if (command == "status" || command == "s") {
      printStatus();
    } else {
      Serial.println("Unknown command. Type 'help' for available commands");
    }
  }
}

void printHelp() {
  Serial.println("\n=== LCD Test Commands ===");
  Serial.println("p      - Pause/Resume tests");
  Serial.println("n      - Next test");
  Serial.println("b      - Toggle backlight");
  Serial.println("c      - Clear display");
  Serial.println("s      - Show status");
  Serial.println("help   - Show this help");
  Serial.println("========================\n");
}

void printStatus() {
  Serial.println("\n=== LCD Test Status ===");
  Serial.print("Current test: ");
  Serial.print(currentTest + 1);
  Serial.println("/14");
  Serial.print("Test paused: ");
  Serial.println(testPaused ? "Yes" : "No");
  Serial.print("Uptime: ");
  Serial.print(millis() / 1000);
  Serial.println(" seconds");
  
  Serial.println("\nLCD Configuration:");
  Serial.print("- Address: 0x");
  Serial.println(LCD_I2C_ADDRESS, HEX);
  Serial.print("- Size: ");
  Serial.print(LCD_COLUMNS);
  Serial.print(" x ");
  Serial.println(LCD_ROWS);
  Serial.println("======================\n");
}