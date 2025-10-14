# Smart LED Clock - Testing Documentation 🧪

> Comprehensive testing suite for hardware and software validation

[![Tests](https://img.shields.io/badge/tests-6%20suites-success.svg)](./test_codes/)
[![Hardware](https://img.shields.io/badge/hardware-verified-green.svg)](./hardware_tests.md)
[![Software](https://img.shields.io/badge/software-validated-green.svg)](./software_tests.md)

[🇫🇷 Version Française](./README_FR.md) | [🇬🇧 English Version](./README.md)

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Test Philosophy](#-test-philosophy)
- [Quick Start](#-quick-start)
- [Test Suites](#-test-suites)
- [Documentation](#-documentation)
- [Test Results Tracking](#-test-results-tracking)

---

## 🎯 Overview

This testing directory contains all the resources needed to validate the Smart LED Clock project components individually before full system integration. Each test is designed to be standalone, easy to run, and provides clear pass/fail indicators.

### Why Individual Testing?

Individual component testing allows you to:
- ✅ **Isolate issues** - Identify which component is causing problems
- ✅ **Verify connections** - Ensure proper wiring before integration
- ✅ **Validate configuration** - Check I2C addresses, pin assignments, etc.
- ✅ **Save time** - Debug components separately rather than the full system
- ✅ **Build confidence** - Know each component works before assembly

---

## 🔬 Test Philosophy

### Testing Approach

Our testing strategy follows these principles:

1. **Simple First** - Start with basic tests (Arduino, LED) before complex ones
2. **Incremental** - Add one component at a time
3. **Visual Feedback** - All tests provide clear serial output and visual indicators
4. **Non-Destructive** - Tests won't damage components
5. **Repeatable** - Run tests multiple times to verify stability
6. **Interactive** - Tests include commands for manual verification

### Test Order Recommendation

```
1. test_arduino      → Verify Arduino board basics (8 tests)
2. test_led          → Test LED strips (4 animation tests)
3. test_lcd          → Test I2C LCD display (14 display tests)
4. test_buttons      → Test button input (click/double-click/long press)
5. test_rtc          → Test RTC module and timekeeping (continuous monitoring)
6. test_sensors      → Test DHT22 and MQ135 sensors (continuous readings)
```

---

## ⚡ Quick Start

### Prerequisites

- **Arduino IDE** 1.8.19 or newer (or Arduino IDE 2.x)
- **Arduino Uno R4 WiFi** connected via USB
- **Required libraries** installed (see each test's requirements)
- **Serial monitor** ready (115200 baud)
- **External power supply** for LED tests (5V/5A recommended)

### Required Libraries

Install via Arduino IDE Library Manager (Sketch → Include Library → Manage Libraries):

```
- Adafruit NeoPixel (by Adafruit)
- LiquidCrystal I2C (by Frank de Brabander)
- RTClib (by Adafruit)
- DHT sensor library (by Adafruit)
- OneButton (by Matthias Hertel)
- WiFiS3 (built-in for Arduino Uno R4 WiFi)
```

### Running a Test

1. **Open the test sketch**
   ```
   File → Open → tests/test_codes/test_led/test_led.ino
   ```

2. **Select your board**
   ```
   Tools → Board → Arduino Uno R4 → Arduino Uno R4 WiFi
   Tools → Port → [select your COM/ttyUSB port]
   ```

3. **Upload the sketch**
   ```
   Sketch → Upload (or Ctrl+U / Cmd+U)
   ```

4. **Open Serial Monitor**
   ```
   Tools → Serial Monitor (or Ctrl+Shift+M / Cmd+Shift+M)
   Set baud rate to 115200
   ```

5. **Observe test results**
   - Watch serial output for status messages
   - Observe physical components (LEDs, LCD, etc.)
   - Follow on-screen instructions for interactive tests

### Expected Outcome

Each test provides:
- 📊 **Serial output** - Detailed status messages and test progress
- 💡 **Visual feedback** - LEDs light up, LCD displays patterns
- ✅ **Pass/Fail indicators** - Clear success/error messages
- 📝 **Troubleshooting hints** - Suggestions if test fails
- 🎮 **Interactive commands** - Type commands to control tests

---

## 🧩 Test Suites

### 1. Arduino Basic Test 🔌
**File:** `test_codes/test_arduino/test_arduino.ino`

**Purpose:** Verify Arduino Uno R4 WiFi board functionality

**Tests performed:**
1. Serial Communication (115200 baud)
2. Built-in LED (rapid blinking test)
3. Digital Pins (OUTPUT and INPUT_PULLUP modes)
4. Analog Pins (ADC readings 0-1023)
5. PWM Pins (0%, 50%, 100% duty cycle)
6. WiFi Module (presence and firmware version)
7. RTC Functionality (basic timing with millis())
8. Memory (1KB allocation test on 32KB SRAM)

**Duration:** ~30 seconds  
**Required Hardware:** Arduino Uno R4 WiFi only  
**Libraries:** WiFiS3 (built-in)

**Serial Commands:** None (automatic test sequence)

**Success Criteria:**
- All 8 tests show "PASS" status
- Built-in LED blinks after tests complete
- "ALL TESTS PASSED" message displayed

---

### 2. LED Strips Test 💡
**File:** `test_codes/test_led/test_led.ino`

**Purpose:** Verify all WS2812B NeoPixel LED strips

**Components tested:**
- 60 LED ring (minutes/seconds) on pin 9
- 12 LED ring (hours) on pin 10
- 10 LED bar (air quality) on pin 11

**Test sequence:**
1. **Rainbow Animation** (5s) - Full color spectrum cycling
2. **Chase Animation** (5s) - Single LED chasing across all strips
3. **Brightness Test** (6s) - 6 levels from 25 to 255
4. **Clock Simulation** (15s) - Simulates 03:10:xx with overlap detection

**⚠️ CRITICAL POWER REQUIREMENT:**
```
USB power is INSUFFICIENT for LED strips!
Required: 5V/5A external power supply
- 60 LEDs: up to 3.6A
- 12 LEDs: up to 0.7A
- 10 LEDs: up to 0.6A
- Total: up to 5A at full brightness
```

**Duration:** 31 seconds (full cycle)  
**Required Hardware:** 3× NeoPixel strips, 5V/5A power supply  
**Libraries:** Adafruit NeoPixel

**Serial Commands:**
- `p` - Pause/Resume tests
- `n` - Next test immediately
- `b+` - Increase brightness
- `b-` - Decrease brightness
- `c` - Clear all LEDs
- `s` - Show status
- `h` - Show help

**Success Criteria:**
- All LEDs light up in all tests
- Colors are vibrant (not dim)
- Smooth animations without flickering
- Clock simulation shows proper overlap at second 10

---

### 3. LCD Display Test 📺
**File:** `test_codes/test_lcd/test_lcd.ino`

**Purpose:** Verify 20×4 I2C LCD display functionality

**Components tested:**
- I2C communication at address 0x27
- 20 columns × 4 rows text display
- Custom character (degree symbol)
- Backlight control

**Test sequence (14 tests, 4s each):**
1. Basic Display - Project name and info
2. All Lines - Test all 4 lines with different content
3. Custom Characters - Degree symbol test (23.5°C)
4. Scrolling Text - Horizontal text scrolling
5. Cursor Positioning - Moving cursor demonstration
6. Numbers and Time - Clock-style display (HH:MM:SS)
7. Special Characters - Symbols: []{}()<>+-*/=
8. Temperature Display - Simulated sensor readings
9. Contrast Test - Readability check (ABCDEFGHIJKLMNOPQRST)
10. Line Clearing - Individual line refresh test
11. Text Alignment - Left/center/right alignment
12. Long Text - Text truncation handling
13. Backlight Control - ON/OFF cycling
14. Stable Display - Anti-flicker test with indicator

**⚠️ POWER WARNING:**
```
LCD backlight draws 80-150mA
Computer USB may be insufficient
RECOMMENDED: External 5V power supply (500mA+)
```

**Duration:** 56 seconds (full cycle)  
**Required Hardware:** 20×4 I2C LCD, external power recommended  
**Libraries:** Wire, LiquidCrystal_I2C

**Serial Commands:**
- `p` - Pause/Resume tests
- `n` - Next test immediately
- `b` - Toggle backlight
- `c` - Clear display
- `s` - Show status
- `h` - Show help

**Success Criteria:**
- LCD address detected in I2C scan (0x27)
- All 4 lines display correctly
- Custom degree symbol appears correctly
- Backlight turns on/off smoothly
- No flickering or random characters

---

### 4. Button Test 🔘
**File:** `test_codes/test_buttons/test_buttons.ino`

**Purpose:** Verify push button functionality and debouncing

**Components tested:**
- Button 1 on digital pin 2
- Button 2 on digital pin 3
- Internal pull-up resistors
- Debouncing algorithm (50ms)

**Event types detected:**
1. **Single Click** - Quick press and release
2. **Double Click** - Two clicks within 400ms
3. **Long Press Start** - Held for 1000ms+ (press detected)
4. **Long Press Continue** - Updates every 500ms while held
5. **Long Press Stop** - Button released after long press

**Timing configuration:**
- Debounce: 50ms (reduces false triggers)
- Click timeout: 400ms (max time for single click)
- Long press: 1000ms (minimum hold time)

**Duration:** Continuous (interactive)  
**Required Hardware:** 2× push buttons, no external resistors needed  
**Libraries:** OneButton

**Serial Commands:**
- `s` - Show current status and counters
- `r` - Reset all counters
- `t` - Run automatic test (hold both buttons 3s)
- `c` - Show button configuration
- `h` - Show help

**Button indicators:**
- 🔸 = Button 1 events
- 🔹 = Button 2 events

**Success Criteria:**
- Single clicks detected reliably
- Double clicks work without false triggers
- Long press starts after exactly 1 second
- Raw state changes printed correctly
- No missed presses or false triggers

---

### 5. RTC Module Test 🕐
**File:** `test_codes/test_rtc/test_rtc.ino`

**Purpose:** Verify DS3231 Real-Time Clock module

**Components tested:**
- I2C communication (address 0x68)
- Time reading and setting
- Built-in temperature sensor
- Battery backup (CR2032)
- Timing accuracy monitoring

**Continuous monitoring:**
- Time reading every 1 second
- Temperature reading every 1 second
- Drift tracking (cumulative seconds)
- Detailed status every 30 seconds

**Duration:** Continuous (monitoring)  
**Required Hardware:** DS3231 RTC module, CR2032 battery  
**Libraries:** Wire, RTClib

**Serial Commands:**
- `s` - Show current status
- `r` - Reset time to compile time
- `t` - Show temperature information
- `d` - Detailed diagnostics
- `b` - Check battery backup status
- `x` - Power loss test (disconnect/reconnect Arduino)
- `h` - Show help

**Monitoring features:**
- Unix timestamp tracking
- Time drift calculation
- Temperature averaging
- Power loss detection
- Day of week validation

**Success Criteria:**
- DS3231 detected at I2C address 0x68
- Time advances correctly (1 second per second)
- Temperature readings reasonable (-50°C to 85°C)
- Battery backup maintains time during power loss
- Drift remains under ±5 seconds over long periods

---

### 6. Environmental Sensors Test 🌡️
**File:** `test_codes/test_sensors/test_sensors.ino`

**Purpose:** Verify DHT22 temperature/humidity sensors and MQ135 air quality sensor

**Components tested:**
- DHT22 Indoor sensor on pin 5
- DHT22 Outdoor sensor on pin 6
- MQ135 Air quality sensor on pin A0

**Measurements provided:**
- **Temperature** - Direct sensor reading (°C)
- **Humidity** - Direct sensor reading (%)
- **Heat Index** - Computed perceived temperature
- **Dew Point** - Calculated using Magnus-Tetens formula
- **Humidex** - Canadian comfort index
- **Air Quality** - Raw ADC value (0-1023) and estimated AQI (0-500)

**Validation ranges:**
- Temperature: -40°C to 80°C
- Humidity: 0% to 100%
- Air Quality ADC: 0 to 1023

**Reading interval:** Every 3 seconds (DHT22 minimum requirement)  
**Duration:** Continuous (monitoring)  
**Required Hardware:** 2× DHT22 sensors, 1× MQ135 sensor  
**Libraries:** DHT

**Error tracking:**
- Indoor sensor error count
- Outdoor sensor error count
- Air quality sensor error count
- Success rate calculations
- Automatic diagnostics

**Display format:**
```
┌──────────────────────────────────────────────────────────────────────┐
│                           SENSOR READINGS                            │
├──────────────────────────────────────────────────────────────────────┤
│ Sensor    │ Temp(°C) │ Hum(%) │ Heat Idx │ Dew Pt │ Humidx │ Status │
├──────────────────────────────────────────────────────────────────────┤
│ Indoor    │    22.5  │  65.0  │    23.1  │  15.8  │   25   │  PASS  │
│ Outdoor   │    18.3  │  56.0  │    18.5  │  9.8   │   19   │  PASS  │
│ Air Qual  │    --    │   --   │    --    │   --   │   --   │  PASS  │
│           │          Raw ADC:  412   Estimated AQI:  82              │
└──────────────────────────────────────────────────────────────────────┘
```

**Status reports:** Every 30 seconds with success rates

**Success Criteria:**
- Both DHT22 sensors provide valid readings
- Temperature and humidity in reasonable ranges
- Calculated values (heat index, dew point, humidex) are plausible
- MQ135 provides stable analog readings
- Error rate under 10% for all sensors
- Success rate above 90%

---

## 📚 Documentation

### Detailed Guides

| Document | Description | Link |
|----------|-------------|------|
| **Hardware Tests** | Step-by-step hardware testing procedures with wiring diagrams | [hardware_tests.md](./hardware_tests.md) |
| **Software Tests** | Software validation, integration tests, and system verification | [software_tests.md](./software_tests.md) |
| **Troubleshooting** | Common issues, solutions, and debugging techniques | [troubleshooting.md](./troubleshooting.md) |

### Additional Resources

- 📘 **[Main Project Documentation](../docs/)** - Complete project documentation
- 🔧 **[Hardware Guide](../docs/HARDWARE.md)** - Wiring diagrams and component specs
- 📗 **[Installation Guide](../docs/INSTALL.md)** - Setup and configuration
- 📕 **[Changelog](../CHANGELOG.md)** - Version history

---

## 📊 Test Results Tracking

### Recording Your Results

We recommend documenting your test results for future reference:

```markdown
## Test Session: [Date]

### Test Environment
- Arduino IDE Version: 2.2.1
- Arduino Board: Uno R4 WiFi
- Power Supply: 5V/5A external adapter
- Ambient Temperature: 22°C

### Arduino Basic Test
- Status: ✅ PASS
- Notes: All 8 tests passed
- Completion Time: 28 seconds

### LED Strips Test
- Status: ✅ PASS
- Notes: All colors vibrant, smooth animations
- Power Supply Used: 5V/5A adapter
- Issues: None

### LCD Display Test
- Status: ⚠️ PARTIAL
- Notes: Had to adjust I2C address to 0x3F
- Custom characters work perfectly
- Backlight control: OK

### Button Test
- Status: ✅ PASS
- Notes: All button events detected correctly
- Debouncing works well
- No false triggers

### RTC Module Test
- Status: ✅ PASS
- Notes: Time accurate, battery backup working
- Temperature: 25.5°C (reasonable)
- Drift: +0.2s after 10 minutes

### Sensors Test
- Status: ✅ PASS
- Notes: Both DHT22 working, MQ135 stable
- Indoor: 22.3°C, 58%
- Outdoor: 18.7°C, 65%
- Success Rate: 100% (no errors)

### Overall Assessment
All tests passed successfully. System ready for integration.
```

### Test Result Badges

Use these badges in your documentation:

- ✅ **PASS** - Test completed successfully
- ⚠️ **PARTIAL** - Test passed with minor issues or adjustments needed
- ❌ **FAIL** - Test failed, troubleshooting required
- ⏸️ **SKIP** - Test not performed (component not available)

---

## 🎓 Best Practices

### Before Testing

1. ✅ Install all required libraries
2. ✅ Check component connections twice
3. ✅ Use external power for high-current tests (LEDs, LCD)
4. ✅ Have multimeter ready for voltage checks
5. ✅ Read the test's specific warnings and requirements

### During Testing

1. 📝 Document unexpected behavior
2. 📊 Note actual values vs expected values
3. 🔄 Run tests multiple times for consistency
4. 📸 Take photos/videos of issues for troubleshooting
5. ⚠️ Stop immediately if you smell burning or see smoke

### After Testing

1. ✅ Save serial monitor output for documentation
2. ✅ Mark components that passed with labels
3. ✅ Set aside components that failed for debugging
4. ✅ Update your test results log
5. ✅ Proceed to next test only when current test passes

---

## 🚀 Next Steps

Once all tests pass:

1. **Proceed to Integration** - Combine all components in main project
2. **Upload Main Firmware** - Install smart-led-clock.ino
3. **Configure WiFi** - Set up secrets.h with your network credentials
4. **Test Full System** - Verify all features work together
5. **Enjoy Your Clock!** - Monitor time, temperature, and air quality

---

## 💡 Tips and Tricks

### Serial Monitor Not Showing Output?
- Check baud rate is set to 115200
- Try unplugging and replugging USB cable
- Close and reopen Serial Monitor
- Check that correct COM port is selected

### Tests Keep Failing?
- Start with simplest test first (test_arduino)
- Check all connections match pin definitions in code
- Verify power supply voltage and current capacity
- Test components individually before combining

### Need Help?
- Check [troubleshooting.md](./troubleshooting.md) for common issues
- Review [hardware_tests.md](./hardware_tests.md) for wiring diagrams
- Consult component datasheets for specifications
- Open an issue on GitHub with details and serial output

---

## 📄 License

These test files are part of the Smart LED Clock project and are licensed under the **MIT License**.

**Author:** F. Baillon  
**Version:** 1.0.0  
**Last Updated:** January 2025