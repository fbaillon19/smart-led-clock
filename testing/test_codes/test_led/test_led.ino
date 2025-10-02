/**
 * Smart LED Clock - LED Strips Test
 * 
 * This test verifies that all WS2812B LED strips are working correctly:
 * - 60 LED ring for minutes/seconds
 * - 12 LED ring for hours  
 * - 10 LED bar for air quality
 * 
 * Tests include:
 * - Individual LED control
 * - Color accuracy (RGB)
 * - Brightness control
 * - Animation patterns
 * - Power consumption estimation
 * 
 * ⚠️  CRITICAL POWER REQUIREMENTS:
 * LED strips draw significant current - USB power is INSUFFICIENT!
 * 
 * Current consumption (estimated):
 * - 60 LEDs at full brightness: ~3.6A
 * - 12 LEDs at full brightness: ~0.7A  
 * - 10 LEDs at full brightness: ~0.6A
 * - Arduino + overhead: ~0.1A
 * - TOTAL: Up to 5A at full white brightness
 * 
 * 🔌 MANDATORY: Use external 5V power supply:
 * - Minimum: 5V/3A power supply
 * - Recommended: 5V/5A power supply
 * - Connect power supply directly to LED strips
 * - Share common ground with Arduino
 * 
 * ⚡ DO NOT run this test on USB power alone!
 * - LEDs may appear dim or not work at all
 * - Arduino may reset randomly
 * - Risk of damaging USB port or computer
 * 
 * Expected Results:
 * - All LEDs should light up in sequence
 * - Colors should be accurate and vibrant
 * - Animations should be smooth
 * - No flickering or dead LEDs
 * - No power supply overheating
 * 
 * Author: F. Baillon
 * License: MIT
 */

#include <Adafruit_NeoPixel.h>

// Pin definitions (match your actual wiring)
#define PIN_LEDS_MINUTE_SECOND  9     // 60 LEDs ring
#define PIN_LEDS_HOUR          10     // 12 LEDs ring  
#define PIN_LEDS_AIR_QUALITY   11     // 10 LEDs bar

// LED counts
#define NUM_LEDS_MINUTE_SECOND  60
#define NUM_LEDS_HOUR          12
#define NUM_LEDS_AIR_QUALITY   10

// Color definitions for clock display (matching Horloge3.ino)
#define COLOR_SECOND_R   0    // Green for seconds
#define COLOR_SECOND_G   127
#define COLOR_SECOND_B   0

#define COLOR_MINUTE_R   127  // Red for minutes
#define COLOR_MINUTE_G   0
#define COLOR_MINUTE_B   0

#define COLOR_HOUR_R     0    // Blue for hours
#define COLOR_HOUR_G     0
#define COLOR_HOUR_B     127

#define COLOR_OVERLAP_R  127  // Yellow for overlap (minute = second)
#define COLOR_OVERLAP_G  127
#define COLOR_OVERLAP_B  0

// Create NeoPixel objects
Adafruit_NeoPixel stripMinuteSec(NUM_LEDS_MINUTE_SECOND, PIN_LEDS_MINUTE_SECOND, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripHour(NUM_LEDS_HOUR, PIN_LEDS_HOUR, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripAirQuality(NUM_LEDS_AIR_QUALITY, PIN_LEDS_AIR_QUALITY, NEO_GRB + NEO_KHZ800);

// Test parameters
int currentTest = 0;
unsigned long lastTestChange = 0;
unsigned long currentTestDuration = 5000;
bool testPaused = false;

// Animation variables
int animationStep = 0;
unsigned long lastAnimationUpdate = 0;
int brightness = 100;

// Test completion tracking
bool testCompleted = false;
int totalCycles = 0;

// Color definitions
struct Color {
  uint8_t r, g, b;
  const char* name;
};

Color testColors[] = {
  {255, 0, 0, "Red"},
  {0, 255, 0, "Green"}, 
  {0, 0, 255, "Blue"},
  {255, 255, 0, "Yellow"},
  {255, 0, 255, "Magenta"},
  {0, 255, 255, "Cyan"},
  {255, 255, 255, "White"}
};

const int numColors = sizeof(testColors) / sizeof(testColors[0]);

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=====================================");
  Serial.println("   Smart LED Clock - LED Strips Test");
  Serial.println("=====================================");
  Serial.println();
  Serial.println("🚨 CRITICAL POWER WARNING 🚨");
  Serial.println("LED strips require HIGH CURRENT!");
  Serial.println();
  Serial.println("Current requirements:");
  Serial.println("  60 LEDs: up to 3.6A");
  Serial.println("  12 LEDs: up to 0.7A");  
  Serial.println("  10 LEDs: up to 0.6A");
  Serial.println("  TOTAL: up to 5A");
  Serial.println();
  Serial.println("⚡ MANDATORY EXTERNAL POWER:");
  Serial.println("  - Use 5V/5A power supply");
  Serial.println("  - Connect power directly to LED strips");
  Serial.println("  - Share common ground with Arduino");
  Serial.println("  - DO NOT rely on USB power alone!");
  Serial.println();
  Serial.println("❌ Without proper power supply:");
  Serial.println("  - LEDs will be dim or not work");
  Serial.println("  - Arduino may reset randomly");
  Serial.println("  - Risk of damaging USB port");
  Serial.println();
  Serial.println("✅ With proper power supply:");
  Serial.println("  - Bright, vibrant colors");
  Serial.println("  - Stable operation");
  Serial.println("  - Full brightness capability");
  Serial.println();
  Serial.println("Continue only if external power connected!");
  Serial.println("Press any key to continue or Ctrl+C to stop...");
  
  // Wait for user confirmation
  while (!Serial.available()) {
    delay(100);
    static unsigned long lastBlink = 0;
    static bool ledState = false;
    if (millis() - lastBlink > 500) {
      digitalWrite(LED_BUILTIN, ledState);
      ledState = !ledState;
      lastBlink = millis();
    }
  }
  
  while (Serial.available()) {
    Serial.read();
  }
  
  Serial.println();
  Serial.println("Proceeding with LED test...");
  Serial.println("=====================================");
  Serial.println();
  
  Serial.println("Initializing LED strips...");
  stripMinuteSec.begin();
  stripHour.begin();
  stripAirQuality.begin();
  
  stripMinuteSec.setBrightness(brightness);
  stripHour.setBrightness(brightness);
  stripAirQuality.setBrightness(brightness);
  
  clearAllStrips();
  
  Serial.print("LED strips initialized:\n- Minute/Second ring: ");
  Serial.print(NUM_LEDS_MINUTE_SECOND);
  Serial.print(" LEDs on pin ");
  Serial.println(PIN_LEDS_MINUTE_SECOND);
  Serial.print("- Hour ring: ");
  Serial.print(NUM_LEDS_HOUR);
  Serial.print(" LEDs on pin ");
  Serial.println(PIN_LEDS_HOUR);
  Serial.print("- Air Quality bar: ");
  Serial.print(NUM_LEDS_AIR_QUALITY);
  Serial.print(" LEDs on pin ");
  Serial.println(PIN_LEDS_AIR_QUALITY);
  Serial.println();
  
  Serial.println("Starting LED test sequence...");
  Serial.println("Tests will run automatically. Send 'p' to pause/resume, 'n' for next test");
  Serial.println("Send 'b+' or 'b-' to adjust brightness");
  Serial.println();
  
  printTestInfo();
  lastTestChange = millis();
}

unsigned long getTestDuration(int testNum) {
  switch (testNum) {
    case 0: return 5000;
    case 1: return 5000;
    case 2: return 6000;
    case 3: return 15000;
    default: return 5000;
  }
}

void loop() {
  handleSerialCommands();
  
  if (!testPaused) {
    if (millis() - lastTestChange >= currentTestDuration || testCompleted) {
      currentTest = (currentTest + 1) % 4;
      lastTestChange = millis();
      currentTestDuration = getTestDuration(currentTest);
      testCompleted = false;
      animationStep = 0;
      totalCycles = 0;
      printTestInfo();
    }
    
    runCurrentTest();
  }
  
  delay(50);
}

void printTestInfo() {
  Serial.print(">>> Test ");
  Serial.print(currentTest + 1);
  Serial.print("/4: ");
  
  switch (currentTest) {
    case 0: 
      Serial.print("Rainbow Animation - All Colors");
      Serial.print(" (");
      Serial.print(currentTestDuration / 1000);
      Serial.println("s)");
      break;
    case 1: 
      Serial.print("Chase Animation - All Strips");
      Serial.print(" (");
      Serial.print(currentTestDuration / 1000);
      Serial.println("s)");
      break;
    case 2: 
      Serial.print("Brightness Test - All Levels");
      Serial.print(" (");
      Serial.print(currentTestDuration / 1000);
      Serial.println("s)");
      break;
    case 3: 
      Serial.print("Clock Simulation - 03:10:xx (Overlapping Test)");
      Serial.print(" (");
      Serial.print(currentTestDuration / 1000);
      Serial.println("s)");
      break;
  }
}

void runCurrentTest() {
  switch (currentTest) {
    case 0: testRainbowAnimation(); break;
    case 1: testChaseAnimation(); break;
    case 2: testBrightness(); break;
    case 3: testClockSimulationFixed(); break;
  }
}

void testRainbowAnimation() {
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastUpdate >= 50) {
    for (int i = 0; i < stripMinuteSec.numPixels(); i++) {
      int hue = (animationStep * 256 / stripMinuteSec.numPixels() + i * 65536L / stripMinuteSec.numPixels()) % 65536;
      stripMinuteSec.setPixelColor(i, stripMinuteSec.gamma32(stripMinuteSec.ColorHSV(hue)));
    }
    stripMinuteSec.show();
    
    stripHour.clear();
    stripHour.show();
    stripAirQuality.clear();
    stripAirQuality.show();
    
    animationStep++;
    lastUpdate = millis();
    
    static unsigned long lastStatus = 0;
    if (millis() - lastStatus >= 1000) {
      Serial.println("🌈 Rainbow animation running - All colors cycling");
      lastStatus = millis();
    }
  }
}

void testChaseAnimation() {
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastUpdate >= 100) {
    clearAllStrips();
    
    int totalLEDs = stripMinuteSec.numPixels() + stripHour.numPixels() + stripAirQuality.numPixels();
    int currentPos = animationStep % totalLEDs;
    
    if (currentPos < stripMinuteSec.numPixels()) {
      stripMinuteSec.setPixelColor(currentPos, 0, 255, 0);
      stripMinuteSec.show();
      Serial.print("Chase: MinSec LED ");
      Serial.println(currentPos);
    } else if (currentPos < stripMinuteSec.numPixels() + stripHour.numPixels()) {
      int pos = currentPos - stripMinuteSec.numPixels();
      stripHour.setPixelColor(pos, 0, 0, 255);
      stripHour.show();
      Serial.print("Chase: Hour LED ");
      Serial.println(pos);
    } else {
      int pos = currentPos - stripMinuteSec.numPixels() - stripHour.numPixels();
      stripAirQuality.setPixelColor(pos, 255, 0, 0);
      stripAirQuality.show();
      Serial.print("Chase: AirQual LED ");
      Serial.println(pos);
    }
    
    animationStep++;
    lastUpdate = millis();
  }
}

void testBrightness() {
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastUpdate >= 1000) {
    int brightnessLevels[] = {25, 50, 100, 150, 200, 255};
    int numLevels = sizeof(brightnessLevels) / sizeof(brightnessLevels[0]);
    
    int currentBrightness = brightnessLevels[animationStep % numLevels];
    
    stripMinuteSec.setBrightness(currentBrightness);
    stripHour.setBrightness(currentBrightness);
    stripAirQuality.setBrightness(currentBrightness);
    
    fillStrip(&stripMinuteSec, COLOR_SECOND_R, COLOR_SECOND_G, COLOR_SECOND_B);
    fillStrip(&stripHour, COLOR_HOUR_R, COLOR_HOUR_G, COLOR_HOUR_B);
    fillStrip(&stripAirQuality, COLOR_MINUTE_R, COLOR_MINUTE_G, COLOR_MINUTE_B);
    
    Serial.print("💡 Brightness level: ");
    Serial.print(currentBrightness);
    Serial.print("/255 (");
    Serial.print((currentBrightness * 100) / 255);
    Serial.println("%) - Colors: Green/Blue/Red");
    
    animationStep++;
    
    if (animationStep >= numLevels) {
      testCompleted = true;
      Serial.println("✅ Brightness test completed - All levels with project colors");
      stripMinuteSec.setBrightness(brightness);
      stripHour.setBrightness(brightness);
      stripAirQuality.setBrightness(brightness);
    }
    
    lastUpdate = millis();
  }
}

void testClockSimulationFixed() {
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastUpdate >= 1000) {
    clearAllStrips();
    
    int hour = 3;
    int minute = 10;
    int second = (animationStep % 60);
    
    stripHour.setPixelColor(hour, COLOR_HOUR_R, COLOR_HOUR_G, COLOR_HOUR_B);
    stripMinuteSec.setPixelColor(minute, COLOR_MINUTE_R, COLOR_MINUTE_G, COLOR_MINUTE_B);
    
    if (minute != second) {
      stripMinuteSec.setPixelColor(second, COLOR_SECOND_R, COLOR_SECOND_G, COLOR_SECOND_B);
    } else {
      stripMinuteSec.setPixelColor(second, COLOR_OVERLAP_R, COLOR_OVERLAP_G, COLOR_OVERLAP_B);
      Serial.println("🔶 OVERLAP! Second and minute at same position (10)");
    }
    
    stripHour.show();
    stripMinuteSec.show();
    stripAirQuality.clear();
    stripAirQuality.show();
    
    Serial.print("🕐 Clock: 03:10:");
    if (second < 10) Serial.print("0");
    Serial.print(second);
    if (second == 10) {
      Serial.print(" (OVERLAPPING!)");
    }
    Serial.println();
    
    animationStep++;
    lastUpdate = millis();
  }
}

void fillStrip(Adafruit_NeoPixel* strip, uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < strip->numPixels(); i++) {
    strip->setPixelColor(i, r, g, b);
  }
  strip->show();
}

void clearAllStrips() {
  stripMinuteSec.clear();
  stripHour.clear();
  stripAirQuality.clear();
  stripMinuteSec.show();
  stripHour.show();
  stripAirQuality.show();
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
      currentTest = (currentTest + 1) % 4;
      lastTestChange = millis();
      animationStep = 0;
      printTestInfo();
    } else if (command == "b+") {
      brightness = min(255, brightness + 25);
      updateBrightness();
      Serial.print("Brightness increased to: ");
      Serial.println(brightness);
    } else if (command == "b-") {
      brightness = max(25, brightness - 25);
      updateBrightness();
      Serial.print("Brightness decreased to: ");
      Serial.println(brightness);
    } else if (command == "help" || command == "h") {
      printHelp();
    } else if (command == "status" || command == "s") {
      printStatus();
    } else if (command == "clear" || command == "c") {
      clearAllStrips();
      Serial.println("All LEDs cleared");
    } else {
      Serial.println("Unknown command. Type 'help' for available commands");
    }
  }
}

void updateBrightness() {
  stripMinuteSec.setBrightness(brightness);
  stripHour.setBrightness(brightness);
  stripAirQuality.setBrightness(brightness);
  
  stripMinuteSec.show();
  stripHour.show();
  stripAirQuality.show();
}

void printHelp() {
  Serial.println("\n=== LED Test Commands ===");
  Serial.println("p      - Pause/Resume tests");
  Serial.println("n      - Next test");
  Serial.println("b+     - Increase brightness");
  Serial.println("b-     - Decrease brightness");
  Serial.println("c      - Clear all LEDs");
  Serial.println("s      - Show status");
  Serial.println("help   - Show this help");
  Serial.println("========================\n");
}

void printStatus() {
  Serial.println("\n=== LED Test Status ===");
  Serial.print("Current test: ");
  Serial.print(currentTest + 1);
  Serial.println("/4");
  Serial.print("Test paused: ");
  Serial.println(testPaused ? "Yes" : "No");
  Serial.print("Brightness: ");
  Serial.print(brightness);
  Serial.println("/255");
  Serial.print("Uptime: ");
  Serial.print(millis() / 1000);
  Serial.println(" seconds");
  
  Serial.println("\nLED Strip Configuration:");
  Serial.print("- Minute/Second: ");
  Serial.print(NUM_LEDS_MINUTE_SECOND);
  Serial.print(" LEDs on pin ");
  Serial.println(PIN_LEDS_MINUTE_SECOND);
  Serial.print("- Hour: ");
  Serial.print(NUM_LEDS_HOUR);
  Serial.print(" LEDs on pin ");
  Serial.println(PIN_LEDS_HOUR);
  Serial.print("- Air Quality: ");
  Serial.print(NUM_LEDS_AIR_QUALITY);
  Serial.print(" LEDs on pin ");
  Serial.println(PIN_LEDS_AIR_QUALITY);
  
  int totalLEDs = NUM_LEDS_MINUTE_SECOND + NUM_LEDS_HOUR + NUM_LEDS_AIR_QUALITY;
  Serial.print("- Total LEDs: ");
  Serial.println(totalLEDs);
  Serial.println("=====================\n");
}