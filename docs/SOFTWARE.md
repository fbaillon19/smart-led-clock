# Software Architecture Guide

This document describes the software architecture, code structure, and programming details of the Smart LED Clock firmware.

## Table of Contents

- [Overview](#overview)
- [Code Structure](#code-structure)
- [Module Descriptions](#module-descriptions)
- [Main Program Flow](#main-program-flow)
- [Key Algorithms](#key-algorithms)
- [Memory Management](#memory-management)
- [Timing and Interrupts](#timing-and-interrupts)

## Overview

The Smart LED Clock firmware is built using modular C++ code organized into separate header (.h) and implementation (.cpp) files. The architecture follows a clear separation of concerns with dedicated modules for each hardware component and functionality.

### Architecture Principles

- **Modular Design:** Each hardware component has its own module
- **Memory Efficiency:** No String objects, static buffers, optimized structures
- **Non-Blocking:** Uses hardware interrupts and timers, no blocking delays
- **Fail-Safe:** Graceful degradation if components fail
- **Configurable:** All settings centralized in config.h

### Development Environment

- **Platform:** Arduino IDE 2.x
- **Board:** Arduino Uno R4 WiFi
- **Language:** C++ (Arduino framework)
- **Memory:** 32KB RAM, 256KB Flash

## Code Structure

### File Organization

```
firmware/smart-led-clock/
├── smart-led-clock.ino      # Main program file
├── config.h                 # Central configuration
├── secrets.h                # WiFi credentials (not in git)
├── secrets.h.template       # Template for secrets.h
│
├── rtc.h / rtc.cpp          # RTC, WiFi, NTP management
├── leds.h / leds.cpp        # LED control and animations
├── display.h / display.cpp  # LCD display management
├── button.h / button.cpp    # Button input handling
├── sensors.h / sensors.cpp  # DHT22 and MQ135 sensors
├── moon.h / moon.cpp        # Moon phase module
├── storage.h / storage.cpp  # EEPROM configuration
├── datalog.h / datalog.cpp  # MQTT data logging
├── webserver.h / webserver.cpp  # Web interface
├── webpage.h                # HTML content (PROGMEM)
└── strings.h                # Localized text strings
```

### Module Dependencies

```
smart-led-clock.ino
    ├── config.h (included by all)
    ├── secrets.h (WiFi credentials)
    │
    ├── rtc.h
    │   └── NTPClient, WiFiS3, RTClib
    │
    ├── leds.h
    │   └── Adafruit_NeoPixel
    │
    ├── display.h
    │   └── LiquidCrystal_I2C
    │
    ├── button.h
    │   └── OneButton
    │
    ├── sensors.h
    │   └── DHT
    │
    ├── moon.h
    │   └── Stepper
    │
    ├── storage.h
    │   └── EEPROM
    │
    ├── datalog.h
    │   └── PubSubClient (MQTT)
    │
    └── webserver.h
        └── WiFiS3, webpage.h
```

## Module Descriptions

### 1. Main Program (smart-led-clock.ino)

**Purpose:** Program entry point, initialization, and main loop coordination

**Key Functions:**
- `setup()`: Initialize all modules in correct order
- `loop()`: Main event loop with non-blocking operations

**Responsibilities:**
- Hardware initialization sequence
- Coordinate module updates
- Handle hardware interrupt flag (secondTicked)
- Manage daily NTP synchronization
- Monitor system health

### 2. Configuration (config.h)

**Purpose:** Central configuration for all system parameters

**Contents:**
- Pin definitions for all hardware
- Timing constants (update intervals, timeouts)
- Color definitions for LEDs
- NTP server settings
- MQTT broker configuration
- Debug mode toggles
- Language selection

**Usage:** Modify this file to adapt hardware connections or adjust behavior

### 3. RTC Module (rtc.h/cpp)

**Purpose:** Real-time clock, WiFi connectivity, and NTP synchronization

**Key Functions:**
```cpp
bool initRTC()              // Initialize DS3231 RTC
bool connectWiFi()          // Connect to WiFi network
bool syncTimeWithNTP()      // Sync time via NTP
DateTime getCurrentTime()   // Get current time
void onSecondTick()         // ISR for 1Hz interrupt
```

**Features:**
- DS3231 RTC with battery backup
- 1Hz hardware interrupt (SQW pin)
- WiFi connection with auto-retry
- Daily NTP synchronization (configurable time)
- Timezone offset support

**Critical Detail:** The `onSecondTick()` ISR is triggered by DS3231 every second, setting the `secondTicked` flag. Main loop checks this flag to update the clock display.

### 4. LED Module (leds.h/cpp)

**Purpose:** Control three NeoPixel LED strips

**Key Functions:**
```cpp
void initLEDs()                  // Initialize LED strips
void updateLEDClock(DateTime)    // Update clock display
void updateAirQualityLEDs()      // Update air quality bar
void startAnimation()            // Begin hourly animation
bool updateAnimation()           // Update animation frame
```

**LED Assignments:**
- **RUB1 (60 LEDs):** Minutes (green) and seconds (red)
- **RUB2 (12 LEDs):** Hours (blue)
- **RUB3 (10 LEDs):** Air quality gradient

**Color Logic:**
- When second and minute coincide: yellow (overlap color)
- Configurable colors via config.h
- Brightness control (0-255)

**Animation:** 5-second color wave animation at each hour

### 5. Display Module (display.h/cpp)

**Purpose:** LCD 20×4 display management

**Key Functions:**
```cpp
void initDisplay()                     // Initialize LCD
void updateLCDDisplay(DateTime)        // Update display
void displayTempHumidity(DateTime)     // Mode 1
void displayFeelsLike(DateTime)        // Mode 2
void displayHumidex(DateTime)          // Mode 3
void manageLCDBacklight()              // Auto-off timer
```

**Display Modes:**
1. Temperature and humidity (indoor/outdoor)
2. Feels-like temperature and dew point
3. Humidex comfort index

**Features:**
- Multi-mode display (cycle with button)
- Automatic backlight timeout (configurable)
- Optimized updates (only refresh changed data)
- Custom degree symbol character
- Multi-language support (strings.h)

### 6. Button Module (button.h/cpp)

**Purpose:** Handle push button input with debouncing

**Key Functions:**
```cpp
void initButton()         // Initialize button with callbacks
void updateButton()       // Poll button state (call in loop)
void buttonClick()        // Callback: single click
void buttonLongPress()    // Callback: long press (>2s)
```

**Behavior:**
- **Single click:** Cycle display mode (or wake LCD if off)
- **Long press:** Return to default mode

**Library:** Uses OneButton library for debouncing and press detection

### 7. Sensors Module (sensors.h/cpp)

**Purpose:** Read DHT22 and MQ135 sensors

**Key Functions:**
```cpp
void initSensors()              // Initialize sensor hardware
void updateSensors()            // Read all sensors
float computeHeatIndex(...)     // Calculate heat index
float computeDewPoint(...)      // Calculate dew point
float computeHumidex(...)       // Calculate humidex
int estimateAQI(int)            // Convert MQ135 to AQI
```

**Data Structures:**
```cpp
struct SensorData {
  float temperature;
  float humidity;
  float heatIndex;
  float dewPoint;
  bool valid;
};

struct AirQualityData {
  int rawValue;
  int estimatedAQI;
  bool valid;
};
```

**Update Interval:** Every 5 seconds (DHT22 requirement)

### 8. Moon Phase Module (moon.h/cpp)

**Purpose:** Calculate and display moon phases using stepper motor

**Key Functions:**
```cpp
bool initMoon()                           // Initialize with calibration
MoonCalibrationResult calibrateMoonHome() // Find home position
bool updateMoonPosition(unsigned long)    // Move to current phase
float calculateLunarAge(unsigned long)    // Calculate moon age
uint8_t calculateMoonPhase(unsigned long) // Get phase number (0-7)
```

**Algorithm:** Hybrid Meeus + average lunar cycle
- Uses Jean Meeus astronomical algorithm for next new moon
- Interpolates position using 29.53-day average cycle
- Precision: ±2 minutes for new moon prediction

**Calibration:**
- Gaussian peak detection using LDR sensor
- Finds alignment hole in moon disk
- Monthly automatic recalibration

**Moon Phases (0-7):**
0. New Moon
1. Waxing Crescent
2. First Quarter
3. Waxing Gibbous
4. Full Moon
5. Waning Gibbous
6. Last Quarter
7. Waning Crescent

### 9. Storage Module (storage.h/cpp)

**Purpose:** Persistent configuration in EEPROM

**Key Functions:**
```cpp
void initStorage()                  // Load or create default config
bool loadConfig(ClockConfig*)       // Load from EEPROM
bool saveConfig(const ClockConfig*) // Save to EEPROM
void applyConfig(const ClockConfig*) // Apply to running system
```

**Stored Parameters:**
- WiFi credentials
- NTP settings (timezone, sync time)
- LED colors and brightness
- LCD timeout
- Language preference
- Moon phase tracking data

**Wear Leveling:** Only writes when values change (expected lifetime: 274+ years at 1 write/day)

### 10. Data Logging Module (datalog.h/cpp)

**Purpose:** MQTT data logging with offline buffering

**Key Functions:**
```cpp
void initDataLog()          // Initialize MQTT client
void handleDataLog()        // Process logging (call in loop)
bool logDataPoint()         // Log current sensor data
bool sendMQTTData()         // Send to MQTT broker
bool flushBufferedData()    // Send buffered data when online
```

**Strategy:**
- **WiFi OK:** Send immediately every 2 minutes (no buffering)
- **WiFi DOWN:** Store in RAM buffer every 5 minutes
- **WiFi RESTORED:** Flush buffered data to MQTT

**Buffer:**
- Capacity: 200 points (16h40min at 5-minute interval)
- Size: 2.8KB RAM
- Circular buffer (oldest overwritten when full)

**MQTT Topics:**
- `home/clock/sensors` - Current sensor data
- `home/clock/buffer` - Buffered data chunks
- `home/clock/status` - System status

### 11. Web Server Module (webserver.h/cpp + webpage.h)

**Purpose:** HTTP web interface for monitoring and configuration

**Key Functions:**
```cpp
void initWebServer()           // Start web server on port 80
void handleWebServer()         // Process HTTP requests
const char* getSensorDataJSON() // Get sensor data as JSON
const char* getConfigJSON()    // Get configuration as JSON
bool parseAndSaveConfig(...)   // Save POSTed configuration
```

**Endpoints:**
- `/` - Main dashboard with sensor data
- `/api/sensors` - JSON sensor data
- `/api/config` - JSON configuration
- `/api/log-stats` - Data logging statistics
- `/api/moon` - Moon phase data and calibration

**Features:**
- Real-time sensor display
- Configuration editor
- MQTT logging statistics
- Moon phase calibration control

**Memory:** HTML stored in PROGMEM to save RAM

## Main Program Flow

### Setup Sequence

```cpp
void setup() {
  1. Serial.begin(115200)           // Debug output
  2. Wire.begin()                   // I2C bus
  3. initStorage()                  // Load EEPROM config
  4. initRTC()                      // DS3231 + SQW interrupt
  5. connectWiFi()                  // WiFi connection
  6. syncTimeWithNTP()              // Initial time sync
  7. initDisplay()                  // LCD display
  8. initLEDs()                     // NeoPixel strips
  9. initButton()                   // Button with OneButton
  10. initSensors()                 // DHT22 + MQ135
  11. initMoon()                    // Moon phase (if enabled)
  12. initDataLog()                 // MQTT client
  13. initWebServer()               // HTTP server
  14. attachInterrupt()             // SQW interrupt → onSecondTick()
}
```

### Main Loop Structure

```cpp
void loop() {
  // Process button input
  updateButton();
  
  // Handle web server requests
  if (wifiConnected) {
    handleWebServer();
  }
  
  // Handle data logging
  handleDataLog();
  
  // Manage LCD backlight timeout
  manageLCDBacklight();
  
  // UPDATE CLOCK ON INTERRUPT
  if (secondTicked && !isAnimationActive && !mqttBusy)) {
    secondTicked = false;  // Reset flag
    
    DateTime now = getCurrentTime();
    
    // Update LED clock display
    updateLEDClock(now);
    
    // Update LCD display
    updateLCDDisplay(now);
    
    // Check for daily NTP sync
    if (now.hour() == NTP_SYNC_HOUR && now.minute() == NTP_SYNC_MINUTE) {
      if (!ntpSyncedToday) {
        syncTimeWithNTP();
        ntpSyncedToday = true;
      }
    }
    
    // Reset daily sync flag at midnight
    if (now.hour() == 0 && now.minute() == 0) {
      ntpSyncedToday = false;
    }
    
    // Check for hourly animation
    if (now.second() == 0 && now.minute() == 0) {
      startAnimation();
    }
  }
  
  // Update animation if active
  if (isAnimationActive) {
    if (!updateAnimation()) {
      stopAnimation();
    }
  }
  
  // Update sensors (every 5 seconds)
  static unsigned long lastSensorUpdate = 0;
  if (millis() - lastSensorUpdate >= SENSOR_UPDATE_INTERVAL) {
    lastSensorUpdate = millis();
    updateSensors();
    updateAirQualityLEDs();
  }
  
  // Update moon phase (every minute)
  static unsigned long lastMoonUpdate = 0;
  if (millis() - lastMoonUpdate >= 60000) {
    lastMoonUpdate = millis();
    if (moonModuleEnabled) {
      updateMoonPosition(now.unixtime());
    }
  }
}
```

## Key Algorithms

### 1. Hardware Interrupt Timing

**Method:** DS3231 SQW pin generates 1Hz square wave

```cpp
volatile bool secondTicked = false;

void onSecondTick() {
  secondTicked = true;  // Set flag (ISR must be fast!)
}

void setup() {
  attachInterrupt(digitalPinToInterrupt(PIN_DS3231_SQW), 
                  onSecondTick, FALLING);
}

void loop() {
  if (secondTicked) {
    secondTicked = false;
    // Update clock...
  }
}
```

**Advantages:**
- Precise 1Hz timing from hardware RTC
- No polling or delays needed
- Minimal CPU overhead

### 2. LED Clock Display Algorithm

```cpp
void updateLEDClock(DateTime now) {
  int hour = now.hour() % 12;
  int minute = now.minute();
  int second = now.second();
  
  // Clear all LEDs
  ledsHour.clear();
  ledsMinuteSec.clear();
  
  // Display hour (12 LEDs)
  ledsHour.setPixelColor(hour, COLOR_HOUR);
  
  // Display minute (60 LEDs)
  ledsMinuteSec.setPixelColor(minute, COLOR_MINUTE);
  
  // Display second (60 LEDs)
  if (second == minute) {
    // Overlap: blend colors
    ledsMinuteSec.setPixelColor(second, COLOR_OVERLAP);
  } else {
    ledsMinuteSec.setPixelColor(second, COLOR_SECOND);
  }
  
  // Update displays
  ledsHour.show();
  ledsMinuteSec.show();
}
```

### 3. Air Quality Visualization

```cpp
void updateAirQualityLEDs() {
  int aqi = airQuality.estimatedAQI;
  
  // Map AQI (0-500) to LED count (0-10)
  int ledCount = map(aqi, 0, 500, 0, 10);
  ledCount = constrain(ledCount, 0, 10);
  
  ledsAirQuality.clear();
  
  for (int i = 0; i < ledCount; i++) {
    // Color gradient: green → yellow → orange → red
    uint32_t color = getAirQualityColor(i, ledCount);
    ledsAirQuality.setPixelColor(i, color);
  }
  
  ledsAirQuality.show();
}
```

### 4. Lunar Age Calculation (Hybrid Meeus)

```cpp
float calculateLunarAge(unsigned long currentEpoch) {
  // If Meeus not initialized, use simple approximation
  if (!moonData.meeusInitialized) {
    const float KNOWN_NEW_MOON = 1609459200; // Jan 13, 2021
    float daysSince = (currentEpoch - KNOWN_NEW_MOON) / 86400.0;
    return fmod(daysSince, LUNAR_CYCLE_DAYS);
  }
  
  // Use Meeus next new moon as reference
  float daysSinceNewMoon = (currentEpoch - moonData.nextNewMoonEpoch) / 86400.0;
  
  // If past next new moon, recalculate
  if (daysSinceNewMoon > LUNAR_CYCLE_DAYS) {
    moonData.nextNewMoonEpoch = calculateNextNewMoon(currentEpoch);
    moonData.lastMeeusSync = currentEpoch;
    daysSinceNewMoon = (currentEpoch - moonData.nextNewMoonEpoch) / 86400.0;
  }
  
  // Wrap to 0-29.53 range
  float age = fmod(daysSinceNewMoon + LUNAR_CYCLE_DAYS, LUNAR_CYCLE_DAYS);
  return age;
}
```

## Memory Management

### RAM Usage Optimization

**Total RAM:** 32KB  
**Used:** ~19.4KB (60%)  
**Free:** ~12.6KB (40%)

**Memory Budget:**
- Global variables: ~8KB
- Data logging buffer: ~2.8KB
- Stack (local variables): ~4KB
- Arduino core: ~2KB
- Library overhead: ~2KB

**Optimization Techniques:**

1. **No String Objects**
```cpp
// BAD (heap fragmentation)
String message = "Temp: " + String(temp) + " C";

// GOOD (static buffer)
char message[32];
snprintf(message, sizeof(message), "Temp: %.1f C", temp);
```

2. **PROGMEM for Constants**
```cpp
// Store HTML in flash instead of RAM
const char WEBPAGE_HOME[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
...
</html>
)rawliteral";
```

3. **Efficient Data Structures**
```cpp
// Compact sensor data (14 bytes per point)
struct DataPoint {
  uint32_t timestamp;     // 4 bytes
  int16_t tempIn;         // 2 bytes (temp * 10)
  uint16_t humIn;         // 2 bytes (humidity * 10)
  int16_t tempOut;        // 2 bytes
  uint16_t humOut;        // 2 bytes
  uint16_t aqi;           // 2 bytes
};
```

4. **Stack Usage**
```cpp
// Avoid large local arrays on stack
void badFunction() {
  char buffer[1024];  // BAD: Uses 1KB of stack
}

// Use static or global for large buffers
static char sharedBuffer[512];  // GOOD: In data segment
```

### Memory Leak Prevention

- **No dynamic allocation:** No malloc/free or new/delete
- **Static buffers:** Fixed-size buffers prevent fragmentation
- **Library selection:** Avoid libraries that use String

## Timing and Interrupts

### Interrupt Service Routine (ISR)

**DS3231 SQW Interrupt:**
```cpp
void onSecondTick() {
  secondTicked = true;  // MUST be very fast (<50µs)
  // NO Serial.print, delay, or blocking operations!
}
```

**ISR Rules:**
- Keep ISR extremely short
- Only modify volatile variables
- No Serial.print() or complex operations
- No delays or blocking calls

### Non-Blocking Operations

All operations in `loop()` use non-blocking patterns:

```cpp
// Pattern 1: State flag from ISR
if (secondTicked) {
  secondTicked = false;
  // Handle event...
}

// Pattern 2: Timed intervals
static unsigned long lastUpdate = 0;
if (millis() - lastUpdate >= INTERVAL) {
  lastUpdate = millis();
  // Perform update...
}

// Pattern 3: State machines for animations
if (isAnimationActive) {
  if (updateAnimation()) {
    // Continue...
  } else {
    stopAnimation();
  }
}
```

### Critical Timing

- **DS3231 Interrupt:** Every 1000ms (±2ppm accuracy)
- **Sensor Update:** Every 5000ms (DHT22 requirement)
- **Data Logging:** 120000ms (WiFi) or 300000ms (offline)
- **LCD Backlight:** Configurable timeout (default: 60000ms)
- **NTP Sync:** Daily at configured time (default: 01:01)

---

**Next Steps:**
- For configuration details, see [Configuration Guide](CONFIGURATION.md)
- For web interface, see [Web Server Guide](WEBSERVER.md)
- For testing procedures, see [Testing Guide](../testing/README.md)
