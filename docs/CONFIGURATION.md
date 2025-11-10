# Configuration Guide

This document provides detailed information about all configurable parameters in the Smart LED Clock project.

## Table of Contents

- [Configuration Files](#configuration-files)
- [Basic Configuration](#basic-configuration)
- [Pin Configuration](#pin-configuration)
- [Network Settings](#network-settings)
- [Time and NTP](#time-and-ntp)
- [LED Configuration](#led-configuration)
- [LCD Display](#lcd-display)
- [Sensors](#sensors)
- [Data Logging (MQTT)](#data-logging-mqtt)
- [Moon Phase Module](#moon-phase-module)
- [EEPROM Storage](#eeprom-storage)
- [Runtime Configuration](#runtime-configuration)

## Configuration Files

### Primary Configuration Files

| File | Purpose | Location | In Git |
|------|---------|----------|--------|
| `config.h` | Main configuration | `firmware/smart-led-clock/` | ✅ Yes |
| `secrets.h` | WiFi credentials | `firmware/smart-led-clock/` | ❌ No (.gitignore) |
| `secrets.h.template` | Template for secrets | `firmware/smart-led-clock/` | ✅ Yes |

### Configuration Hierarchy

1. **Compile-time:** `config.h` defines defaults
2. **Credentials:** `secrets.h` provides WiFi/MQTT passwords
3. **Persistent:** EEPROM stores runtime changes
4. **Runtime:** Web interface allows live updates

## Basic Configuration

### Project Information

Located in `config.h`:

```cpp
#define PROJECT_VERSION_MAJOR 1
#define PROJECT_VERSION_MINOR 2
#define PROJECT_VERSION_PATCH 0
#define PROJECT_VERSION_STRING "1.2.0"
```

### Debug Mode

```cpp
#define DEBUG_MODE 1  // 1 = enabled, 0 = disabled
```

**When enabled:**
- Serial output at 115200 baud
- Detailed logging messages
- Memory usage reports

**When disabled:**
- No serial overhead
- Slightly faster execution
- Reduced code size

**Usage:**
```cpp
DEBUG_PRINTLN("This only prints if DEBUG_MODE is 1");
```

### Language Selection

```cpp
#define LANG_FR 0
#define LANG_EN 1
#define LANGUAGE LANG_FR  // Change to LANG_EN for English
```

**Affects:**
- LCD display text
- Web interface labels
- Serial messages (partially)

**Supported Languages:**
- French (LANG_FR)
- English (LANG_EN)

## Pin Configuration

All pin assignments are in `config.h`. Modify these if your wiring differs.

### Digital Pins

```cpp
// LED Strips
#define PIN_LEDS_MINUTE_SECOND  9   // 60-LED ring (minutes/seconds)
#define PIN_LEDS_HOUR          10   // 12-LED ring (hours)
#define PIN_LEDS_AIR_QUALITY   11   // 10-LED bar (air quality)

// Sensors
#define PIN_DHT_INDOOR          5   // DHT22 indoor sensor
#define PIN_DHT_OUTDOOR         6   // DHT22 outdoor sensor

// RTC Interrupt
#define PIN_DS3231_SQW          2   // 1Hz square wave from DS3231

// Button
#define PIN_BUTTON             13   // Mode switch button

// Moon Phase Module
#define PIN_MOON_STEPPER_IN1    3   // ULN2003 IN1
#define PIN_MOON_STEPPER_IN2    4   // ULN2003 IN2
#define PIN_MOON_STEPPER_IN3    7   // ULN2003 IN3
#define PIN_MOON_STEPPER_IN4    8   // ULN2003 IN4
#define PIN_MOON_CALIB_LED     12   // Calibration indicator LED
```

### Analog Pins

```cpp
#define PIN_AIR_QUALITY_SENSOR  A0  // MQ135 analog input
#define PIN_MOON_LDR_SENSOR     A1  // LDR for moon calibration
// A4 = I2C SDA (DS3231, LCD)
// A5 = I2C SCL (DS3231, LCD)
```

**⚠️ Important Notes:**
- Pin 2 must support interrupts (for DS3231 SQW)
- Pins 9, 10, 11 should support PWM (for NeoPixels)
- A4/A5 are reserved for I2C (cannot be changed)

## Network Settings

### WiFi Credentials

**File:** `secrets.h` (create from `secrets.h.template`)

```cpp
// secrets.cpp content:
#include "secrets.h"

const char* ssid = "YourWiFiSSID";
const char* pass = "YourWiFiPassword";
```

**Requirements:**
- **2.4GHz network only** (Arduino R4 WiFi limitation)
- WPA/WPA2 security supported
- WEP not recommended

**Security:**
- Never commit `secrets.h` to version control
- Already in `.gitignore`
- Use strong passwords

### MQTT Broker Settings

Located in `config.h`:

```cpp
// Enable/disable MQTT
#define MQTT_ENABLED true  // Set to false to disable

// Broker connection
#define MQTT_SERVER "192.168.1.21"  // IP or hostname
#define MQTT_PORT 1883              // Standard MQTT port
#define MQTT_CLIENT_ID "smart-led-clock"

// Authentication (if required by broker)
#define MQTT_USERNAME "smart-led-clock"
#define MQTT_PASSWORD "mqtt-password"

// Topics
#define MQTT_TOPIC_DATA   "home/clock/sensors"  // Real-time data
#define MQTT_TOPIC_BUFFER "home/clock/buffer"   // Buffered data
#define MQTT_TOPIC_STATUS "home/clock/status"   // System status
```

**Topics Structure:**
- `home/clock/sensors` - Current sensor readings (JSON)
- `home/clock/buffer` - Buffered data when WiFi was offline
- `home/clock/status` - System status and diagnostics

## Time and NTP

### Timezone Configuration

```cpp
#define TIME_ZONE_OFFSET 1  // Hours offset from UTC
```

**Examples:**
- `0` = UTC (London in winter)
- `1` = UTC+1 (Paris in winter, CET)
- `2` = UTC+2 (Paris in summer, CEST)
- `-5` = UTC-5 (New York in winter, EST)
- `-4` = UTC-4 (New York in summer, EDT)

**Note:** Does not handle DST automatically. Update manually for daylight saving time.

### Daily NTP Synchronization

```cpp
#define NTP_SYNC_HOUR   1   // Hour for daily sync (0-23)
#define NTP_SYNC_MINUTE 1   // Minute for daily sync (0-59)
```

**Default:** 01:01 AM (minimal user impact)

**How it works:**
1. At specified time, clock connects to NTP server
2. Gets precise UTC time from pool.ntp.org
3. Applies timezone offset
4. Updates DS3231 RTC
5. Continues normal operation

**Accuracy:** Typically ±50ms

### Manual Time Setting

If NTP is unavailable, time can be set manually:

```cpp
// In setup() or via web interface
rtc.adjust(DateTime(2025, 1, 15, 14, 30, 0));
// Format: (year, month, day, hour, minute, second)
```

## LED Configuration

### LED Strip Counts

```cpp
#define NUM_LEDS_HOUR           12  // Hour ring
#define NUM_LEDS_MINUTE_SECOND  60  // Minute/second ring
#define NUM_LEDS_AIR_QUALITY    10  // Air quality bar
```

**Note:** Must match your physical LED strips

### LED Colors (RGB Values 0-255)

#### Hour Hand Color

```cpp
#define COLOR_HOUR_R    0    // Red component
#define COLOR_HOUR_G    0    // Green component
#define COLOR_HOUR_B  255    // Blue component
// Result: Blue
```

#### Minute Hand Color

```cpp
#define COLOR_MINUTE_R    0    // Red
#define COLOR_MINUTE_G  255    // Green
#define COLOR_MINUTE_B    0    // Blue
// Result: Green
```

#### Second Hand Color

```cpp
#define COLOR_SECOND_R    255  // Red
#define COLOR_SECOND_G      0  // Green
#define COLOR_SECOND_B      0  // Blue
// Result: Red
```

#### Overlap Color

```cpp
#define COLOR_OVERLAP_R   255  // Red
#define COLOR_OVERLAP_G   255  // Green
#define COLOR_OVERLAP_B     0  // Blue
// Result: Yellow (when second and minute overlap)
```

### LED Brightness

```cpp
// Global brightness (0-255)
uint8_t ledBrightness = 100;  // Default: ~40% brightness
```

**Recommendations:**
- `50` = 20% (subtle, energy-efficient)
- `100` = 40% (good balance)
- `150` = 60% (bright)
- `255` = 100% (maximum, high power draw)

**Power Consideration:** Full brightness can draw 5A+. Use appropriate power supply.

### Color Customization Examples

**Warm Colors:**
```cpp
// Hour: Orange
#define COLOR_HOUR_R  255
#define COLOR_HOUR_G  100
#define COLOR_HOUR_B    0

// Minute: Yellow
#define COLOR_MINUTE_R  255
#define COLOR_MINUTE_G  200
#define COLOR_MINUTE_B    0

// Second: Red
#define COLOR_SECOND_R  255
#define COLOR_SECOND_G    0
#define COLOR_SECOND_B    0
```

**Cool Colors:**
```cpp
// Hour: Cyan
#define COLOR_HOUR_R    0
#define COLOR_HOUR_G  255
#define COLOR_HOUR_B  255

// Minute: Blue
#define COLOR_MINUTE_R    0
#define COLOR_MINUTE_G    0
#define COLOR_MINUTE_B  255

// Second: Purple
#define COLOR_SECOND_R  128
#define COLOR_SECOND_G    0
#define COLOR_SECOND_B  255
```

## LCD Display

### Hardware Configuration

```cpp
#define LCD_I2C_ADDRESS 0x27  // I2C address
#define LCD_COLUMNS     20    // Character columns
#define LCD_ROWS        4     // Character rows
```

**Finding I2C Address:**
If LCD not working, run I2C scanner test to find address:
```bash
testing/test_codes/test_arduino/test_arduino.ino
```

Common addresses: `0x27` or `0x3F`

### Backlight Timeout

```cpp
#define LCD_BACKLIGHT_TIMEOUT 30000  // milliseconds
```

**Values:**
- `30000` = 30 seconds (default)
- `60000` = 1 minute
- `300000` = 5 minutes
- `0` = Never turn off (not recommended - screen burn-in)

**Behavior:**
- Backlight turns off after timeout
- Single button press wakes up (no mode change)
- Subsequent presses cycle modes

### Display Modes

The LCD cycles through three display modes:

1. **MODE_TEMP_HUMIDITY** (default)
   - Indoor/outdoor temperature
   - Indoor/outdoor humidity

2. **MODE_FEELS_LIKE**
   - Feels-like temperature
   - Dew point

3. **MODE_HUMIDEX**
   - Humidex comfort index
   - Interpretation

**Cycle:** Button → Mode 1 → Mode 2 → Mode 3 → Mode 1...

## Sensors

### DHT22 Configuration

```cpp
#define DHT_TYPE DHT22
#define SENSOR_UPDATE_INTERVAL 5000  // 5 seconds
```

**Requirements:**
- Minimum 2 seconds between readings (DHT22 spec)
- 10kΩ pull-up resistor on data line (required)
- 3.3V or 5V power supply

**Note:** 5-second interval provides good balance between responsiveness and sensor longevity.

### MQ135 Air Quality Sensor

```cpp
#define PIN_AIR_QUALITY_SENSOR A0
```

**Calibration:** Requires 24-48 hour burn-in period for accurate readings.

**AQI Estimation:** Raw analog value (0-1023) is converted to estimated AQI (0-500):
- 0-50: Good (Green)
- 51-100: Moderate (Yellow)
- 101-150: Unhealthy for sensitive groups (Orange)
- 151-200: Unhealthy (Red)
- 201-300: Very unhealthy (Purple)
- 301+: Hazardous (Maroon)

**Note:** This is an estimation. For precise AQI, professional calibration required.

## Data Logging (MQTT)

### Logging Strategy

The clock uses adaptive logging:

**WiFi Connected:**
```cpp
#define DATALOG_INTERVAL_WIFI_OK 120000  // 2 minutes
```
- Logs immediately to MQTT
- No local buffering

**WiFi Disconnected:**
```cpp
#define DATALOG_INTERVAL_WIFI_DOWN 300000  // 5 minutes
```
- Logs to RAM buffer
- Capacity: 200 points (16h40min)
- Auto-flushes when WiFi restored

### Buffer Configuration

```cpp
#define MAX_DATA_POINTS 200     // Number of data points
#define MQTT_CHUNK_SIZE 10      // Points per MQTT message
```

**Buffer Size:** 200 points × 14 bytes = 2.8 KB RAM

**Capacity Calculation:**
- 200 points × 5 minutes = 1000 minutes = 16h40min
- Sufficient for typical network outages

### Connection Settings

```cpp
#define MQTT_CONNECT_TIMEOUT 2000   // 2 seconds
#define MQTT_SOCKET_TIMEOUT  2      // 2 seconds
#define MQTT_RETRY_INTERVAL  30000  // 30 seconds
```

**Behavior:**
- Quick timeout prevents long blocking
- Retries every 30 seconds when disconnected
- Doesn't interfere with clock operation

### Data Format

**JSON Structure:**
```json
{
  "timestamp": "2025-01-15T14:30:00",
  "indoor": {
    "temperature": 21.5,
    "humidity": 45.0,
    "heatIndex": 22.1,
    "dewPoint": 9.8
  },
  "outdoor": {
    "temperature": 15.2,
    "humidity": 65.0,
    "heatIndex": 15.8,
    "dewPoint": 8.5
  },
  "airQuality": {
    "raw": 325,
    "aqi": 75
  }
}
```

## Moon Phase Module

### Enable/Disable

```cpp
bool moonModuleEnabled = true;  // Set to false to disable
```

**When disabled:**
- Stepper motor not initialized
- No moon calculations
- Saves power and boot time

### Motor Configuration

```cpp
#define MOON_STEPS_PER_REV 2048     // 28BYJ-48 spec
#define MOON_MOTOR_SPEED_NORMAL 10  // RPM
#define MOON_MOTOR_SPEED_FAST 15    // RPM for calibration
```

**28BYJ-48 Specifications:**
- Steps per revolution: 2048 (with gear reduction)
- Gear ratio: 64:1
- Voltage: 5V DC

### Calibration Settings

```cpp
#define MOON_LDR_THRESHOLD 600      // Light threshold
#define MOON_CALIBRATION_STEP 8     // Steps between readings
```

**Gaussian Peak Detection:**
- Scans full rotation
- Finds brightness peak (alignment hole)
- Sets as new moon reference position

**Frequency:** Automatic monthly recalibration

### Lunar Cycle

```cpp
#define LUNAR_CYCLE_DAYS 29.530588  // Average synodic month
```

**Algorithm:** Hybrid Meeus + average cycle
- Jean Meeus algorithm for next new moon (±2 min accuracy)
- Interpolates between using average 29.53-day cycle
- Updates monthly for drift correction

## EEPROM Storage

### Storage Structure

```cpp
struct ClockConfig {
  uint16_t magic;              // Validation: 0xC10C
  
  // Network
  char wifiSSID[32];
  char wifiPassword[64];
  
  // Time
  int8_t timezoneOffset;
  uint8_t ntpSyncHour;
  uint8_t ntpSyncMinute;
  
  // LEDs
  uint8_t colorHourR, colorHourG, colorHourB;
  uint8_t colorMinuteR, colorMinuteG, colorMinuteB;
  uint8_t colorSecondR, colorSecondG, colorSecondB;
  uint8_t ledBrightness;
  
  // Display
  uint32_t lcdTimeout;
  uint8_t language;
  
  // System
  uint8_t debugMode;
  
  // Moon phase tracking
  unsigned long nextNewMoonEpoch;
  unsigned long lastMeeusSync;
  uint8_t moonModuleEnabled;
  
  // Integrity
  uint16_t checksum;
};
```

**Total Size:** ~180 bytes (of 8KB available)

### Wear Leveling

**Write Protection:**
- Only writes when values actually change
- Checksum validation prevents corrupted data
- Expected lifetime: 274+ years at 1 write/day

**Write Cycle Limit:** 100,000 writes per cell (manufacturer spec)

### Default Configuration

On first boot or corrupted EEPROM, default config is created from `config.h` values.

## Runtime Configuration

### Web Interface

Access at `http://<arduino-ip-address>/config`

**Configurable Parameters:**
- Timezone offset
- NTP sync time
- LED colors (RGB values)
- LED brightness
- LCD timeout
- Moon module enable/disable

**Changes:**
- Applied immediately
- Saved to EEPROM automatically
- Persist across reboots

### Configuration API

**GET Config:**
```
GET /api/config
Returns: JSON configuration
```

**POST Config:**
```
POST /config
Content-Type: application/x-www-form-urlencoded
Body: timezoneOffset=1&ledBrightness=100&...
Returns: Success/error message
```

### Command Line Configuration

For advanced users, configuration can be modified via serial terminal:

1. Connect serial terminal (115200 baud)
2. Modify values in code
3. Re-upload firmware

**Example:**
```cpp
// In setup(), after initStorage():
ClockConfig config;
loadConfig(&config);
config.ledBrightness = 150;  // Change brightness
saveConfig(&config);
applyConfig(&config);
```

## Configuration Checklist

### First-Time Setup

- [ ] Create `secrets.h` from template
- [ ] Set WiFi SSID and password
- [ ] Configure timezone offset
- [ ] Adjust LED colors (optional)
- [ ] Set LED brightness
- [ ] Configure MQTT broker (if using)
- [ ] Set NTP sync time
- [ ] Choose language (French/English)
- [ ] Enable/disable debug mode
- [ ] Verify all pin connections match config.h

### Testing Configuration

After changes:

1. Upload firmware
2. Open Serial Monitor (115200 baud)
3. Verify configuration loaded correctly
4. Check WiFi connection
5. Test NTP synchronization
6. Verify LED colors and brightness
7. Test LCD display modes
8. Check sensor readings
9. Verify MQTT connection (if enabled)
10. Test moon phase calibration (if enabled)

---

**Next Steps:**
- For web interface details, see [Web Server Guide](WEBSERVER.md)
- For testing procedures, see [Testing Guide](../testing/README.md)
- For software architecture, see [Software Architecture](SOFTWARE.md)
