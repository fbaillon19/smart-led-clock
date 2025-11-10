# Testing Guide

This document provides comprehensive testing procedures for the Smart LED Clock project. Each hardware module should be tested individually before integration into the main firmware.

## Table of Contents

- [Overview](#overview)
- [Testing Philosophy](#testing-philosophy)
- [Testing Sequence](#testing-sequence)
- [Test Programs](#test-programs)
- [Testing Procedures](#testing-procedures)
- [Troubleshooting](#troubleshooting)
- [Integration Testing](#integration-testing)

## Overview

The testing directory contains standalone test programs for each hardware module. These tests help verify that components are properly connected and functioning before running the complete firmware.

### Test Directory Structure

```
testing/test_codes/
├── test_arduino/      # Basic Arduino functionality
├── test_led/          # NeoPixel LED strips
├── test_lcd/          # LCD 20×4 I2C display
├── test_button/       # Push button input
├── test_rtc/          # DS3231 RTC with SQW interrupt
├── test_sensors/      # DHT22 and MQ135 sensors
└── test_moon_phase/   # Stepper motor and moon phase
```

### Benefits of Module Testing

- **Isolate Issues:** Identify hardware problems before complex integration
- **Verify Connections:** Confirm correct wiring for each component
- **Understand Behavior:** Learn how each module operates independently
- **Save Time:** Fix issues early rather than debugging integrated system
- **Document Baseline:** Establish known-good configurations

## Testing Philosophy

### Test-Driven Development

1. **Test hardware first:** Verify physical connections
2. **Test modules individually:** One component at a time
3. **Test incrementally:** Add modules in logical order
4. **Test integration:** Verify modules work together
5. **Test edge cases:** Unusual conditions and error handling

### Sequential Testing

Tests should be performed in this specific order:

1. **Arduino** - Basic board functionality
2. **LEDs** - Visual output verification
3. **LCD** - Display and I2C communication
4. **Button** - Input handling
5. **RTC** - Timekeeping and interrupts
6. **Sensors** - Environmental data acquisition
7. **Moon Phase** - Complex stepper motor control

**Rationale:** Each test builds on previous successes, creating a solid foundation.

## Testing Sequence

### Pre-Testing Checklist

Before beginning any tests:

- [ ] Arduino IDE installed and configured
- [ ] Arduino Uno R4 WiFi board selected
- [ ] Correct COM port selected
- [ ] All required libraries installed
- [ ] Serial Monitor set to 115200 baud
- [ ] Power supply adequate for components being tested
- [ ] Wiring diagram or schematic available
- [ ] Multimeter available for voltage checks

### Recommended Order

```
1. test_arduino      → Verify Arduino board basics
2. test_led          → Verify LED strips and power
3. test_lcd          → Verify I2C and display
4. test_button       → Verify input handling
5. test_rtc          → Verify timekeeping
6. test_sensors      → Verify environmental sensors
7. test_moon_phase   → Verify motor control
8. Main firmware     → Full integration test
```

**Total Testing Time:** 1-2 hours (depending on issues found)

## Test Programs

### 1. Arduino Basic Test (`test_arduino`)

**Purpose:** Verify core Arduino functionality

**Tests:**
- Serial communication
- Built-in LED control
- Digital pin I/O
- Analog pin reading
- PWM output
- WiFi module presence
- I2C bus scan
- Memory availability

**Duration:** 5-10 minutes

**How to Run:**
```bash
1. Open testing/test_codes/test_arduino/test_arduino.ino
2. Upload to Arduino R4 WiFi
3. Open Serial Monitor (115200 baud)
4. Review test results
```

**Expected Output:**
```
=====================================
   Arduino Basic Functionality Test
   Smart LED Clock Project
=====================================

Testing Serial Communication... PASS
Testing Built-in LED... PASS
Testing Digital Pins... PASS
Testing Analog Pins... PASS
Testing PWM Pins... PASS
Testing WiFi Module... PASS
  - WiFi module detected, firmware: 1.4.8
  - MAC address: XX:XX:XX:XX:XX:XX
Testing I2C Bus Scan... PASS
  Found 2 I2C device(s) at addresses: 0x27 0x68

Test Summary:
  Total Tests: 8
  Passed: 8
  Failed: 0
  Success Rate: 100%

✓ ALL TESTS PASSED!
```

**Success Criteria:**
- All 8 tests pass
- I2C devices found at expected addresses (0x27 and 0x68)
- WiFi module detected
- No error messages

**Common Issues:**
- **I2C devices not found:** Check wiring, verify power
- **WiFi module error:** Update R4 WiFi firmware
- **Serial not working:** Check baud rate (115200)

### 2. LED Test (`test_led`)

**Purpose:** Verify NeoPixel LED strips

**Tests:**
- All three LED strips independently
- Individual pixel control
- Color accuracy (R, G, B)
- Brightness levels
- Animation smoothness
- Power requirements

**Hardware Required:**
- 3× WS2812B LED strips connected
- External 5V power supply (recommended)
- Common ground between Arduino and PSU

**Duration:** 10-15 minutes

**How to Run:**
```bash
1. Open testing/test_codes/test_led/test_led.ino
2. Connect LED strips:
   - RUB1 (60 LEDs) → D9
   - RUB2 (12 LEDs) → D10
   - RUB3 (10 LEDs) → D11
3. Connect external 5V power to LED strips
4. Ensure common ground
5. Upload and monitor Serial output
```

**Test Sequence:**
1. Single LED test (each strip)
2. All LEDs on (white)
3. Color test (R, G, B, Cyan, Magenta, Yellow)
4. Rainbow animation
5. Chase animation
6. Brightness levels (25%, 50%, 75%, 100%)

**Expected Results:**
- All pixels light up
- Colors accurate and consistent
- Smooth animations
- No flickering
- Correct strip counts: 60, 12, 10 LEDs

**Success Criteria:**
- All three strips functional
- All pixels addressable
- Colors match expectations
- No power issues

**Common Issues:**
- **First LED works, rest don't:** Check data line continuity
- **Wrong colors:** Check wiring polarity (DIN vs DOUT)
- **Flickering:** Insufficient power, add capacitor
- **LEDs too dim/bright:** Adjust brightness in code

### 3. LCD Test (`test_lcd`)

**Purpose:** Verify LCD 20×4 I2C display

**Tests:**
- I2C communication
- All 4 lines of text
- Character display
- Custom characters
- Backlight control
- Cursor positioning
- Special characters
- Scrolling text
- Anti-flicker techniques

**Hardware Required:**
- LCD 20×4 with I2C backpack
- Connected to A4 (SDA) and A5 (SCL)

**Duration:** 15-20 minutes

**How to Run:**
```bash
1. Open testing/test_codes/test_lcd/test_lcd.ino
2. Verify I2C address in code (default: 0x27)
3. Upload to Arduino
4. Watch LCD display cycle through tests
5. Monitor Serial output for instructions
```

**Test Sequence (14 tests):**
1. Basic display
2. All lines
3. Custom characters
4. Scrolling text
5. Cursor positioning
6. Numbers and time
7. Special characters
8. Temperature simulation
9. Contrast test
10. Line clearing
11. Text alignment
12. Long text handling
13. Backlight control
14. Stable display (anti-flicker)

**Expected Results:**
- All 4 lines display correctly
- Text clear and readable
- Backlight toggles
- No flickering
- Special characters render properly

**Success Criteria:**
- All 14 tests display correctly
- Text readable at all positions
- Backlight responsive
- No I2C errors

**Common Issues:**
- **Blank display:** Check contrast pot on back of module
- **Wrong address:** Run I2C scanner, update address in code
- **Garbled text:** Check I2C connections, pull-up resistors
- **Flickering:** Reduce refresh rate, use stable display technique

**I2C Address Detection:**
If LCD not working, modify code to scan for address:
```cpp
// In setup():
Wire.begin();
for (byte i = 0; i < 127; i++) {
  Wire.beginTransmission(i);
  if (Wire.endTransmission() == 0) {
    Serial.print("Found device at 0x");
    Serial.println(i, HEX);
  }
}
```

### 4. Button Test (`test_button`)

**Purpose:** Verify push button input and debouncing

**Tests:**
- Button press detection
- Click count
- Long press detection
- Double click detection
- Debouncing effectiveness
- Internal pull-up resistor

**Hardware Required:**
- Push button connected to D13
- Button to GND (uses internal pull-up)

**Duration:** 5 minutes

**How to Run:**
```bash
1. Open testing/test_codes/test_button/test_button.ino
2. Connect button between D13 and GND
3. Upload to Arduino
4. Press button in different ways
5. Monitor Serial output
```

**Test Procedure:**
1. Single clicks (count should increment)
2. Long press (hold >2 seconds)
3. Double click (two quick clicks)
4. Rapid clicking (verify debouncing)

**Expected Output:**
```
Button Test - Press button to test
Click #1
Click #2
Long press detected! (held 2.5s)
Double click detected!
Click #3
```

**Success Criteria:**
- Reliable click detection
- Long press works (>2 seconds)
- Double click registered
- No false triggers from bouncing

**Common Issues:**
- **No detection:** Check button wiring, ensure connection to GND
- **Multiple clicks per press:** Debouncing issue, check library
- **Long press not working:** Increase hold time threshold

### 5. RTC Test (`test_rtc`)

**Purpose:** Verify DS3231 Real-Time Clock and 1Hz interrupt

**Tests:**
- I2C communication with DS3231
- Time reading and setting
- Date/time accuracy
- SQW 1Hz interrupt generation
- Interrupt frequency measurement
- Power loss detection
- Temperature sensor reading

**Hardware Required:**
- DS3231 RTC module
- Battery installed (CR2032)
- SQW pin connected to D2

**Duration:** 10-15 minutes

**How to Run:**
```bash
1. Open testing/test_codes/test_rtc/test_rtc.ino
2. Connect DS3231:
   - VCC → 5V
   - GND → GND
   - SDA → A4
   - SCL → A5
   - SQW → D2
3. Upload to Arduino
4. Monitor Serial output for 60+ seconds
```

**Test Procedure:**
1. RTC initialization check
2. Power loss detection
3. SQW interrupt configuration (1Hz)
4. Interrupt frequency measurement (60 seconds)
5. Time accuracy verification
6. Temperature reading

**Expected Output:**
```
DS3231 RTC Test WITH SQW INTERRUPT

I2C device found at address 0x68
✓ DS3231 RTC initialized successfully
✓ DS3231 maintained time through power cycles
Configuring DS3231 for 1Hz square wave output...
SQW pin initial state: HIGH
Interrupt attached to D2 (FALLING edge)

Testing SQW interrupt for 60 seconds...
Tick  1 - 1000ms since last - Time: 14:35:01
Tick  2 - 1000ms since last - Time: 14:35:02
...
Tick 60 - 1000ms since last - Time: 14:35:60

Interrupt Test Results:
  Expected ticks: 60
  Received ticks: 60
  Success rate: 100%
  ✓ Timing precision: ±2ms
```

**Success Criteria:**
- RTC found at address 0x68
- 60 interrupts in 60 seconds (±1)
- Timing accuracy within ±5ms
- Time updates correctly
- Temperature reading reasonable (20-30°C typical)

**Common Issues:**
- **RTC not found:** Check I2C wiring, verify address
- **No interrupts:** Check SQW pin connection to D2
- **Time resets:** Replace battery, check power connections
- **Inaccurate timing:** Verify DS3231 genuine (not fake), check temperature

### 6. Sensors Test (`test_sensors`)

**Purpose:** Verify DHT22 and MQ135 sensors

**Tests:**
- DHT22 indoor sensor (temperature + humidity)
- DHT22 outdoor sensor (temperature + humidity)
- MQ135 air quality sensor (analog reading)
- Calculated values (heat index, dew point, humidex)
- Data validity checks
- Error rate monitoring

**Hardware Required:**
- 2× DHT22 sensors
- 1× MQ135 air quality sensor
- Pull-up resistors (10kΩ) for DHT22

**Duration:** 10-15 minutes

**How to Run:**
```bash
1. Open testing/test_codes/test_sensors/test_sensors.ino
2. Connect sensors:
   - DHT22 indoor → D5 (with 10kΩ pull-up)
   - DHT22 outdoor → D6 (with 10kΩ pull-up)
   - MQ135 → A0
3. Upload to Arduino
4. Monitor readings for 5-10 minutes
5. Note error rates
```

**Test Output:**
```
Smart LED Clock - Sensors Test

Initializing sensors...
DHT22 Indoor initialized on pin 5
DHT22 Outdoor initialized on pin 6
MQ135 initialized on pin A0
Sensors ready! Reading every 3 seconds...

Reading #1:
┌─────────────────┬─────────┬──────────┬────────┬───────┬──────┐
│ Sensor          │ Temp(°C)│ Humidity │ H.Index│ DewPt │ Valid│
├─────────────────┼─────────┼──────────┼────────┼───────┼──────┤
│ DHT Indoor      │  21.5   │   45.0   │  22.1  │  9.8  │  ✓   │
│ DHT Outdoor     │  15.2   │   65.0   │  15.8  │  8.5  │  ✓   │
│ Air Quality(MQ) │   325   │    -     │   -    │   -   │  ✓   │
└─────────────────┴─────────┴──────────┴────────┴───────┴──────┘

Reading #2:
...
```

**Success Criteria:**
- Both DHT22 sensors return valid data (>95% success)
- Temperatures reasonable (-10°C to 50°C typical)
- Humidity in range (0-100%)
- MQ135 values change with air conditions
- Error rate <10%

**Common Issues:**
- **NaN readings:** Check pull-up resistors, verify wiring
- **High error rate:** Reduce reading frequency (5+ seconds)
- **Constant values:** Faulty sensor, replace
- **MQ135 unstable:** Allow 24-48h burn-in period

**MQ135 Burn-In:**
For accurate air quality readings, MQ135 requires 24-48 hours of continuous operation before stabilizing.

### 7. Moon Phase Test (`test_moon_phase`)

**Purpose:** Verify stepper motor and calibration system

**Tests:**
- Motor basic rotation
- LDR sensor reading
- Calibration LED indicator
- Gaussian peak calibration
- Home position detection
- Continuous rotation
- Position tracking
- Phase calculations

**Hardware Required:**
- 28BYJ-48 stepper motor
- ULN2003 driver board
- LDR photoresistor (with 10kΩ pull-down)
- Calibration LED (with 220Ω resistor)
- Moon phase disk with alignment hole

**Duration:** 15-20 minutes

**How to Run:**
```bash
1. Open testing/test_codes/test_moon_phase/test_moon_phase.ino
2. Connect hardware:
   - ULN2003 IN1-IN4 → D3, D4, D7, D8
   - LDR → A1 (with pull-down to GND)
   - Calibration LED → D12 (with resistor to GND)
3. Mount moon disk on motor shaft
4. Upload to Arduino
5. Follow interactive menu
```

**Test Menu:**
```
================================================
   Smart LED Clock - Moon Phase Module Test
================================================

Test Menu:
1. Motor basic test (full revolution)
2. LDR & LED functionality test
3. Gaussian peak calibration
4. Continuous rotation demo
5. Display current position & status
0. Run all tests automatically

Enter test number (0-5):
```

**Test Procedure:**

**Test 1: Motor Basic**
- Full 360° rotation (2048 steps)
- Duration: ~5 seconds
- Verify smooth rotation
- Check for stalling or skipping

**Test 2: LDR & LED**
- LED turns on
- LDR readings displayed
- Cover sensor with hand (reading should drop)
- Uncover (reading should increase)
- LED turns off

**Test 3: Calibration (Most Important)**
- Motor rotates slowly (~2 minutes)
- LDR scans for brightness peak
- Calibration LED blinks during search
- Peak detected (alignment hole)
- Motor moves to home position
- LED stays on for verification

**Expected Calibration Output:**
```
=== TEST 3: Gaussian Peak Calibration ===
[CALIB] Starting automatic calibration...
[CALIB] This will take ~2 minutes for full scan

Step   | LDR Value | Current Peak | Status
-------|-----------|--------------|------------------
   0   |  245      |  245         | 
 256   |  312      |  312         | 
 512   |  485      |  485         | Near peak
 640   |  723      |  723         | Near peak
 704   |  850      |  850         | ← NEW PEAK! 🎯
 768   |  812      |  850         | Near peak
...
2048   |  243      |  850         |

[CALIB] ✓ Scan complete!
[CALIB] Peak brightness: 850 ADC at step 704
[CALIB] Moving to home position (new moon reference)...
[CALIB] ✓ Home position reached!
```

**Test 4: Continuous Rotation**
- Accelerated lunar cycle (29.5 days in 30 seconds)
- Phase names displayed
- Press any key to stop

**Test 5: Status Display**
- Calibration status
- Current position
- LDR sensor state
- Hardware configuration

**Success Criteria:**
- Motor rotates smoothly
- Calibration finds alignment hole
- Peak detection reliable (>800 ADC)
- Home position accurate
- No motor overheating

**Common Issues:**
- **Motor not moving:** Check ULN2003 connections, verify power
- **Erratic movement:** Incorrect wire sequence, check pin order
- **No peak found:** Adjust LDR position, verify hole in disk
- **Motor overheating:** Coils not disabled after movement
- **Weak LDR signal:** Check pull-down resistor, improve lighting

**Calibration Tips:**
- Ensure alignment hole is clean and unobstructed
- LDR should be positioned to detect light through hole
- Room should have consistent ambient lighting
- Peak value typically >700 ADC for reliable detection

## Testing Procedures

### General Testing Guidelines

**For All Tests:**

1. **Read Documentation:** Review test comments in source code
2. **Check Wiring:** Verify connections before powering on
3. **Serial Monitor:** Always monitor at 115200 baud
4. **Document Results:** Note any issues or anomalies
5. **Repeat if Uncertain:** Run tests multiple times for consistency

### Safety Precautions

⚠️ **Important Safety Rules:**

- Disconnect power before changing wiring
- Verify polarity before connecting power
- Use external power for LEDs (not Arduino 5V pin)
- Common ground for all power sources
- Avoid touching components during operation
- Monitor for overheating (especially motor, MQ135)
- Keep flammable materials away from heated components

### Troubleshooting Workflow

When a test fails:

```
1. Check Physical Connections
   └─> Verify wiring against pin definitions
       └─> Check for loose connections
           └─> Measure voltages with multimeter

2. Check Software Configuration
   └─> Verify correct board selected
       └─> Confirm libraries installed
           └─> Check Serial Monitor baud rate

3. Isolate the Problem
   └─> Test component individually
       └─> Swap with known-good component
           └─> Test on breadboard before PCB

4. Check Documentation
   └─> Review datasheet specifications
       └─> Check example code
           └─> Search for known issues

5. Seek Help
   └─> Check GitHub issues
       └─> Ask in Arduino forums
           └─> Contact component manufacturer
```

## Troubleshooting

### Common Hardware Issues

**I2C Communication Failures**

**Symptoms:**
- Devices not detected
- Garbled data
- Intermittent connection

**Solutions:**
- Check pull-up resistors (4.7kΩ to 5V)
- Shorten I2C wires (<30cm recommended)
- Use twisted pair for SDA/SCL
- Verify power supply quality
- Check for address conflicts

**Power Supply Issues**

**Symptoms:**
- Random resets
- Brownouts
- LEDs flickering
- Sensors returning invalid data

**Solutions:**
- Use adequate power supply (5V 5A minimum)
- Add bulk capacitor (1000µF) at power input
- Common ground for all components
- Measure voltage under load
- Check for voltage drops in wiring

**Sensor Reading Errors**

**Symptoms:**
- NaN or invalid readings
- Constant zero values
- Erratic data

**Solutions:**
- Verify sensor power (3.3V or 5V as required)
- Check data pin connections
- Add/verify pull-up resistors
- Increase delay between readings
- Replace faulty sensor

### Common Software Issues

**Upload Errors**

**Solutions:**
- Select correct board (Arduino Uno R4 WiFi)
- Select correct COM port
- Close Serial Monitor during upload
- Press reset button if stuck
- Reinstall USB drivers

**Library Conflicts**

**Solutions:**
- Update all libraries to latest version
- Check for duplicate libraries
- Verify library compatibility
- Remove unused libraries

**Compilation Errors**

**Solutions:**
- Check board package installed
- Verify all required libraries present
- Update Arduino IDE
- Check for typos in configuration

## Integration Testing

After all individual module tests pass, test the complete firmware.

### Full System Test Procedure

1. **Upload Main Firmware**
   ```bash
   Open firmware/smart-led-clock/smart-led-clock.ino
   Upload to Arduino
   ```

2. **Monitor Startup**
   - Open Serial Monitor (115200 baud)
   - Verify all modules initialize
   - Check for error messages
   - Note WiFi IP address

3. **Verify Core Functions**
   - LED clock displays time
   - LCD shows sensor data
   - Button cycles display modes
   - Sensors update every 5 seconds
   - Moon phase moves (if enabled)

4. **Test Network Features**
   - Connect to web interface
   - Verify sensor data display
   - Test configuration changes
   - Check MQTT logging (if enabled)

5. **Long-Term Testing**
   - Run for 24+ hours
   - Monitor for memory leaks
   - Verify NTP sync
   - Check data logging
   - Ensure no crashes

### Integration Checklist

- [ ] All hardware modules functional
- [ ] Software compiles without errors
- [ ] All libraries installed
- [ ] Configuration customized (WiFi, timezone, etc.)
- [ ] LED colors configured
- [ ] Display modes working
- [ ] Button input responsive
- [ ] RTC keeping accurate time
- [ ] Sensors reading correctly
- [ ] Moon phase tracking (if enabled)
- [ ] Web interface accessible
- [ ] MQTT logging functional (if enabled)
- [ ] System stable for 24+ hours

## Success Criteria Summary

| Test | Duration | Critical Success Factors |
|------|----------|------------------------|
| Arduino | 5-10 min | 100% test pass rate |
| LED | 10-15 min | All strips functional, correct colors |
| LCD | 15-20 min | All 4 lines display correctly |
| Button | 5 min | Reliable click/long press detection |
| RTC | 10-15 min | 60 interrupts in 60 seconds ±1 |
| Sensors | 10-15 min | <10% error rate on readings |
| Moon Phase | 15-20 min | Successful calibration, smooth rotation |
| **Total** | **70-100 min** | **All modules pass** |

---

**Congratulations!** If all tests pass, your Smart LED Clock hardware is fully validated and ready for normal operation. Proceed to calibration and final assembly.

**Need Help?**
- Check [Hardware Guide](../docs/HARDWARE.md) for wiring details
- Review [Software Architecture](../docs/SOFTWARE.md) for code understanding
- See [Configuration Guide](../docs/CONFIGURATION.md) for settings
- Open GitHub issue for unresolved problems
