# Hardware Tests - Smart LED Clock 🔧

> Detailed hardware testing procedures and wiring verification

This document provides step-by-step instructions for testing each hardware component individually before system integration.

---

## 📋 Table of Contents

- [Prerequisites](#prerequisites)
- [Test Sequence](#test-sequence)
- [Individual Component Tests](#individual-component-tests)
  - [1. Arduino Board Test](#1-arduino-board-test)
  - [2. LED Strips Test](#2-led-strips-test)
  - [3. LCD Display Test](#3-lcd-display-test)
  - [4. Button Test](#4-button-test)
  - [5. RTC Module Test](#5-rtc-module-test)
  - [6. Sensors Test](#6-sensors-test)
- [Wiring Diagrams](#wiring-diagrams)
- [Power Requirements](#power-requirements)

---

## Prerequisites

### Required Tools
- 🔌 Arduino Uno R4 WiFi
- 💻 Computer with Arduino IDE installed
- 🔌 USB-C cable for Arduino
- ⚡ 5V power supply (5A recommended for LED tests)
- 🔧 Multimeter (for voltage/continuity testing)
- 📏 Wire strippers and connectors
- 🧰 Small screwdriver set

### Required Libraries
```
Adafruit NeoPixel
LiquidCrystal I2C
RTClib  
DHT sensor library
OneButton
WiFiS3 (built-in)
```

Install via: `Sketch → Include Library → Manage Libraries`

---

## Test Sequence

**Recommended order** (simplest to most complex):

```
1. Arduino Board (baseline functionality)
   ↓
2. LED Strips (visual output, power requirements)
   ↓
3. LCD Display (I2C communication)
   ↓
4. Buttons (input handling)
   ↓
5. RTC Module (I2C, timekeeping)
   ↓
6. Sensors (analog/digital sensing)
```

---

## Individual Component Tests

## 1. Arduino Board Test

### Purpose
Verify that the Arduino Uno R4 WiFi is functioning correctly before connecting any external components.

### Components Needed
- Arduino Uno R4 WiFi
- USB-C cable only

### Wiring
No external wiring needed - this tests the board itself.

### Test Procedure

1. **Upload the test sketch**
   ```
   File → Open → test_arduino.ino
   Tools → Board → Arduino Uno R4 WiFi
   Tools → Port → [select port]
   Sketch → Upload
   ```

2. **Open Serial Monitor**
   ```
   Tools → Serial Monitor
   Set baud rate: 115200
   ```

3. **Observe test sequence**
   The test will automatically perform 8 tests:
   - ✅ Serial Communication
   - ✅ Built-in LED (watch it blink rapidly)
   - ✅ Digital Pins
   - ✅ Analog Pins
   - ✅ PWM Pins
   - ✅ WiFi Module
   - ✅ RTC (basic timing)
   - ✅ Memory

4. **Verify results**
   - All tests should show "PASS"
   - Built-in LED should blink steadily after tests
   - Serial monitor shows "ALL TESTS PASSED!"

### Expected Output
```
=====================================
    Arduino UNO R4 WiFi Basic Test
=====================================

Testing Serial Communication... PASS
  - Serial baud rate: 115200
  - Communication established successfully

Testing Built-in LED... PASS
  - LED pin configured successfully
  - LED blink test completed

Testing Digital Pins... PASS
  - Digital output: OK
  - Digital input with pullup: OK

Testing Analog Pins... PASS
  - Analog readings: 512, 510, 511

Testing PWM Pins... PASS
  - PWM output levels: 0%, 50%, 100% tested

Testing WiFi Module... PASS
  - WiFi module detected, firmware: 0.2.0
  - MAC address: XX:XX:XX:XX:XX:XX

Testing RTC (Real-Time Clock)... PASS
  - Basic timing functionality working
  - Current uptime: 5 seconds

Testing Memory... PASS
  - Memory allocation test successful
  - Arduino UNO R4 has 32KB SRAM total

=====================================
           TEST SUMMARY
=====================================
Total tests run: 8
Tests passed: 8
Tests failed: 0

✅ ALL TESTS PASSED!
Your Arduino UNO R4 WiFi is working correctly.
You can proceed with the Smart LED Clock project.
```

### Troubleshooting
| Problem | Solution |
|---------|----------|
| No serial output | Check baud rate (115200), try different USB port |
| WiFi module fail | Normal if WiFi firmware not updated, won't affect project |
| Memory test fail | May indicate corrupted board, try re-uploading |
| LED doesn't blink | Check board selection, verify it's Uno R4 WiFi |

---

## 2. LED Strips Test

### Purpose
Verify all three NeoPixel LED strips work correctly with proper colors and brightness.

### Components Needed
- Arduino Uno R4 WiFi
- 60 LED NeoPixel ring/strip
- 12 LED NeoPixel ring
- 10 LED NeoPixel bar
- **5V/5A power supply (MANDATORY!)**
- Breadboard and jumper wires

### ⚠️ CRITICAL POWER WARNING

**DO NOT attempt this test with USB power alone!**

```
USB Power (max ~500mA):
  ❌ Insufficient for LED strips
  ❌ LEDs will be dim or not work
  ❌ Arduino may reset randomly
  ❌ Risk of damaging USB port

External 5V/5A Power:
  ✅ Bright, vibrant colors
  ✅ Stable operation
  ✅ Full brightness capability
  ✅ Safe for all components
```

### Wiring Diagram

```
External 5V/5A Power Supply:
  (+5V) ──┬──→ 60 LED strip VCC
          ├──→ 12 LED ring VCC
          ├──→ 10 LED bar VCC
          └──→ Arduino Vin (if needed)
  
  (GND) ──┴──→ All LED GNDs + Arduino GND (common ground!)

Arduino Digital Pins:
  Pin 9  ──→ 60 LED strip Data In
  Pin 10 ──→ 12 LED ring Data In
  Pin 11 ──→ 10 LED bar Data In

LED Strip Connections:
  Strip → Data Out → (not used, or to next strip)
```

**Important Notes:**
- Power supply **must** be connected directly to LED strips
- Arduino and LEDs **must** share common ground
- Data lines run from Arduino to each strip's Data In
- Respect LED strip directionality (arrow on strip)

### Test Procedure

1. **Connect external power**
   - Connect 5V power supply to breadboard
   - Connect all LED strip VCC pins to +5V rail
   - Connect all GND pins together (including Arduino GND)

2. **Connect data lines**
   - Pin 9 → 60 LED strip Data In
   - Pin 10 → 12 LED ring Data In
   - Pin 11 → 10 LED bar Data In

3. **Double-check connections**
   - Verify voltage at LED VCC pins (should be ~5V)
   - Check continuity between all GND points
   - Confirm data pins connected to correct strips

4. **Upload test sketch**
   ```
   File → Open → test_led.ino
   Sketch → Upload
   ```

5. **Confirm power connection**
   Serial monitor will show power warning
   Press any key to continue after external power connected

6. **Observe test sequence**
   - Test 1: Rainbow animation (5s) - All colors cycle
   - Test 2: Chase animation (5s) - LED chases across strips
   - Test 3: Brightness test (6s) - 6 brightness levels
   - Test 4: Clock simulation (15s) - Shows time 03:10:xx

### Serial Commands
While tests are running:
- `p` - Pause/resume automatic testing
- `n` - Skip to next test immediately
- `b+` - Increase brightness
- `b-` - Decrease brightness
- `c` - Clear all LEDs (turn off)
- `s` - Show current status
- `h` - Show help

### Expected Behavior

**Test 1 - Rainbow Animation:**
- All 60 LEDs on minute/second ring cycle through colors
- Hour ring and air quality bar are off
- Full rainbow spectrum visible

**Test 2 - Chase Animation:**
- Single LED chases across all three strips in sequence
- Green LED on 60-LED strip
- Blue LED on 12-LED ring
- Red LED on 10-LED bar

**Test 3 - Brightness Test:**
- All LEDs light up in project colors:
  - 60-LED ring: Green (seconds color)
  - 12-LED ring: Blue (hours color)
  - 10-LED bar: Red (minutes color)
- Cycles through 6 brightness levels:
  25, 50, 100, 150, 200, 255

**Test 4 - Clock Simulation:**
- Simulates time 03:10:xx
- Hour 3 (blue) on 12-LED ring at position 3
- Minute 10 (red) on 60-LED ring at position 10
- Second (green) advances 0-59
- At second=10: **yellow overlap** (minute+second same position)

### Success Criteria
- ✅ All LEDs light up (none are dead)
- ✅ Colors are vibrant and accurate
- ✅ Animations are smooth (no flickering)
- ✅ Overlap test shows yellow at second 10
- ✅ No Arduino resets during test
- ✅ Power supply doesn't overheat

### Troubleshooting
| Problem | Solution |
|---------|----------|
| LEDs very dim | Check power supply - USB is insufficient |
| Some LEDs don't light | Check data line connections, verify directionality |
| Colors wrong (e.g. red shows green) | Strip may be NEO_GRB vs NEO_RGB, check datasheet |
| Flickering | Check power supply capacity and wire gauge |
| First few LEDs work, rest don't | Data line interrupted, check connections |
| Arduino resets | Power supply insufficient or poor ground connection |

---

## 3. LCD Display Test

### Purpose
Verify the 20×4 I2C LCD display communicates correctly and shows text clearly.

### Components Needed
- Arduino Uno R4 WiFi
- 20×4 I2C LCD display (HD44780 controller)
- 4 jumper wires
- External 5V power supply (recommended)

### ⚠️ POWER CONSIDERATION

```
LCD with backlight draws 80-150mA
Computer USB: May be insufficient
External power: Recommended for stable operation

Symptoms of insufficient power:
- Display resets randomly
- Backlight flickers
- Random characters appear
```

### Wiring Diagram

```
LCD I2C Module → Arduino Uno R4:
  VCC → 5V (external power recommended)
  GND → GND
  SDA → A4 (SDA pin)
  SCL → A5 (SCL pin)

I2C Pull-up Resistors:
  Usually built into I2C module (4.7kΩ)
  If not present, add external 4.7kΩ resistors:
    SDA → 5V
    SCL → 5V
```

**Finding Your I2C Address:**
Most common addresses:
- 0x27 (most common)
- 0x3F (some modules)

The test will scan and report the detected address.

### Test Procedure

1. **Connect LCD to Arduino**
   - VCC → 5V
   - GND → GND
   - SDA → A4
   - SCL → A5

2. **Check backlight potentiometer**
   - Locate blue potentiometer on back of LCD module
   - Adjust if display is too dark/light after power on

3. **Upload test sketch**
   ```
   File → Open → test_lcd.ino
   Sketch → Upload
   Tools → Serial Monitor (115200 baud)
   ```

4. **Observe I2C scan results**
   Serial monitor will show detected I2C devices:
   ```
   Scanning for I2C devices...
   I2C device found at address 0x27
   Found 1 I2C device(s)
   ```

5. **Watch 14 test sequence**
   Each test runs for 4 seconds automatically:
   1. Basic Display
   2. All Lines Test
   3. Custom Characters (degree symbol)
   4. Scrolling Text
   5. Cursor Positioning
   6. Numbers and Time
   7. Special Characters
   8. Temperature Display
   9. Contrast Test
   10. Line Clearing
   11. Text Alignment
   12. Long Text Handling
   13. Backlight Control
   14. Stable Display (anti-flicker)

### Serial Commands
- `p` - Pause/resume tests
- `n` - Next test immediately
- `b` - Toggle backlight on/off
- `c` - Clear display
- `s` - Show status
- `h` - Show help

### Expected Behavior

**Test 1 - Basic Display:**
```
Line 0: Smart LED Clock
Line 1: LCD Test - Basic
Line 2: 20x4 I2C Display
Line 3: Test 1/14 - OK!
```

**Test 3 - Custom Characters:**
```
Line 0: Custom Characters
Line 1: Temperature: 23.5°C  ← Degree symbol
Line 2: Humidity: 65%
Line 3: Test 3/14 - Custom
```

**Test 13 - Backlight Control:**
- Backlight turns ON for 1.5s
- Backlight turns OFF for 1.5s
- Repeats cycling
- Status line shows: "Status: ON " or "Status: OFF"

### Success Criteria
- ✅ I2C address detected (0x27 or 0x3F)
- ✅ All 4 lines display correctly
- ✅ Text is readable and sharp
- ✅ Custom degree symbol (°) appears correctly
- ✅ Backlight can be turned on/off
- ✅ No flickering or random characters
- ✅ Text doesn't "ghost" or fade

### Troubleshooting
| Problem | Solution |
|---------|----------|
| No I2C device found | Check SDA/SCL connections, verify power |
| Display shows blocks | Adjust contrast potentiometer on back |
| Garbled text | Wrong I2C address - try 0x3F if 0x27 fails |
| Random characters | Insufficient power - use external 5V supply |
| Backlight doesn't work | Check backlight jumper on module back |
| Display resets | Power issue - check connections and supply |

### I2C Address Change

If your LCD is at 0x3F instead of 0x27:

1. Open test_lcd.ino
2. Find line: `#define LCD_I2C_ADDRESS 0x27`
3. Change to: `#define LCD_I2C_ADDRESS 0x3F`
4. Re-upload sketch

---

## 4. Button Test

### Purpose
Verify push buttons work correctly with proper debouncing and event detection.

### Components Needed
- Arduino Uno R4 WiFi
- 2× Push buttons (normally open)
- 2× jumper wires
- Breadboard (optional)

### Wiring Diagram

```
Button 1:
  One leg → Arduino Pin 2
  Other leg → GND

Button 2:
  One leg → Arduino Pin 3
  Other leg → GND

Notes:
- NO external resistors needed
- Uses internal INPUT_PULLUP
- Button press = LOW signal
- Button release = HIGH signal
```

**Why No Resistors?**
Arduino's INPUT_PULLUP mode activates internal 20-50kΩ pull-up resistors, eliminating the need for external components.

### Button Types Supported
- Tactile push buttons (6mm, 12mm)
- Momentary switches
- Any normally-open (NO) switch

### Test Procedure

1. **Connect buttons**
   - Button 1: One leg to Pin 2, other to GND
   - Button 2: One leg to Pin 3, other to GND

2. **Upload test sketch**
   ```
   File → Open → test_buttons.ino
   Sketch → Upload
   Tools → Serial Monitor (115200 baud)
   ```

3. **Test Button 1 (Pin 2)**
   - **Single click:** Press and release quickly
     - Should print: "🔸 Button 1 CLICK #1"
   
   - **Double click:** Click twice quickly (< 400ms apart)
     - Should print: "🔸🔸 Button 1 DOUBLE-CLICK #1"
   
   - **Long press:** Hold for 1+ second
     - Should print: "🔸⏳ Button 1 LONG PRESS started..."
     - While holding: "🔸⏳ Button 1 LONG PRESS continuing..."
     - On release: "🔸⏹️ Button 1 LONG PRESS stopped #1"

4. **Test Button 2 (Pin 3)**
   Same tests, but with Button 2
   - Single click: "🔹 Button 2 CLICK #X"
   - Double click: "🔹🔹 Button 2 DOUBLE-CLICK #X"
   - Long press: "🔹⏳ Button 2 LONG PRESS..."

5. **Run automatic test** (optional)
   - Send 't' in serial monitor
   - Press and hold BOTH buttons for 3 seconds
   - Test verifies both buttons work simultaneously

### Serial Commands
- `s` - Show status (total clicks, double-clicks, long presses)
- `r` - Reset all counters to zero
- `t` - Run automatic test (hold both buttons 3s)
- `c` - Show button configuration
- `h` - Show help

### Timing Configuration
```
Debounce time: 50ms
  → Eliminates contact bounce
  → Prevents false triggers

Click timeout: 400ms
  → Maximum time for single click
  → Affects double-click detection window

Long press time: 1000ms
  → Minimum hold time for long press
  → Press must be held this long to trigger
```

### Expected Output Examples

**Single Click:**
```
Button 1 raw state: PRESSED
Button 1 raw state: RELEASED
🔸 Button 1 CLICK #1
```

**Double Click:**
```
Button 1 raw state: PRESSED
Button 1 raw state: RELEASED
Button 1 raw state: PRESSED
Button 1 raw state: RELEASED
🔸🔸 Button 1 DOUBLE-CLICK #1
```

**Long Press:**
```
Button 1 raw state: PRESSED
🔸⏳ Button 1 LONG PRESS started...
🔸⏳ Button 1 LONG PRESS continuing...
🔸⏳ Button 1 LONG PRESS continuing...
Button 1 raw state: RELEASED
🔸⏹️ Button 1 LONG PRESS stopped #1
```

### Success Criteria
- ✅ Single clicks detected reliably
- ✅ Double clicks work without accidental singles
- ✅ Long press triggers after exactly 1 second
- ✅ Raw state changes print correctly (PRESSED/RELEASED)
- ✅ No missed button presses
- ✅ No false triggers from electrical noise

### Troubleshooting
| Problem | Solution |
|---------|----------|
| No response from button | Check connections, verify button is working |
| Double-clicking too hard | Normal - adjust timing or press lighter |
| Long press triggers immediately | Check button isn't stuck/shorted |
| Random triggers | Bad button, try different one |
| Missed presses | Button may be worn out or poor contact |

---

## 5. RTC Module Test

### Purpose
Verify DS3231 Real-Time Clock maintains accurate time with battery backup.

### Components Needed
- Arduino Uno R4 WiFi
- DS3231 RTC module
- CR2032 battery (3V lithium)
- 4 jumper wires

### Wiring Diagram

```
DS3231 RTC → Arduino Uno R4:
  VCC → 5V (or 3.3V)
  GND → GND
  SDA → A4 (SDA)
  SCL → A5 (SCL)

Battery:
  CR2032 → Install in DS3231 module battery holder
  (Maintains time when main power is off)

I2C Address: 0x68 (fixed, not configurable)
```

### Battery Installation
1. Locate CR2032 battery holder on DS3231 module
2. Insert CR2032 battery (+ side up, usually marked)
3. Battery life: 8-10 years typically
4. Module will maintain time even when Arduino is unplugged

### Test Procedure

1. **Connect RTC module**
   - VCC → 5V, GND → GND
   - SDA → A4, SCL → A5

2. **Install CR2032 battery**
   - Ensures time is maintained during power cycles

3. **Upload test sketch**
   ```
   File → Open → test_rtc.ino
   Sketch → Upload
   Tools → Serial Monitor (115200 baud)
   ```

4. **Observe I2C scan**
   Should detect DS3231 at address 0x68:
   ```
   Scanning I2C bus for devices...
   I2C device found at address 0x68 (DS3231 RTC)
   Found 1 I2C device(s)
   ```

5. **Check power status**
   ```
   ✅ DS3231 maintained time through power cycles
   OR
   ⚠️ WARNING: DS3231 lost power! Time may be incorrect
   ```

6. **Monitor continuous operation**
   - Time updates every second
   - Temperature reading every second
   - Drift tracking accumulates
   - Detailed report every 30 seconds

### Serial Commands
- `s` - Show current status (time, uptime, test cycles, drift)
- `r` - Reset time to compile time
- `t` - Show temperature information
- `d` - Detailed diagnostics (day of week, timestamp validation)
- `b` - Check battery backup status
- `x` - Power loss test (disconnect/reconnect Arduino)
- `h` - Show help

### Monitoring Output

**Every 10 seconds:**
```
Time: 2025/01/15 14:32:45 | Temp: 25.5°C | Cumulative drift: 0s
```

**Every 30 seconds:**
```
================================
    DETAILED RTC DIAGNOSTICS
================================
📅 Date: 15/1/2025
🕐 Time: 14:32:45
🌡️ Temperature: 25.5°C
   Average temp: 25.3°C
⏱️ Timing accuracy: Excellent (±1s)
🔋 Power status: ✅ Stable
📊 Test statistics: 180 cycles, 180 seconds uptime
================================
```

### Power Loss Test Procedure

Test battery backup functionality:

1. Send `x` command in serial monitor
2. Note the time displayed
3. Disconnect USB cable for 10+ seconds
4. Reconnect USB cable
5. Open serial monitor again
6. Send `x` command again

**Expected result:**
```
✅ PASSED: DS3231 maintained time during power loss!
   - Battery backup working correctly
   - Time continued advancing normally
```

### Temperature Sensor

DS3231 has built-in temperature sensor:
- Range: -40°C to +85°C
- Accuracy: ±3°C
- Purpose: Crystal oscillator compensation
- Updates: Every 64 seconds internally
- Used automatically for timing accuracy

### Success Criteria
- ✅ DS3231 detected at I2C address 0x68
- ✅ Time advances 1 second per second (±1s drift acceptable)
- ✅ Temperature readings reasonable (room temp ±10°C)
- ✅ Battery backup maintains time during power loss
- ✅ No "lost power" warning after battery installed
- ✅ Time persists after Arduino reset
- ✅ Drift stays under ±5 seconds over 10 minutes

### Expected Accuracy
- **DS3231 Specs:** ±2ppm (±1 minute per year)
- **Test results:** Should show <5s drift over 10 minutes
- **Temperature effect:** ±0.5ppm per °C from 25°C

### Troubleshooting
| Problem | Solution |
|---------|----------|
| DS3231 not found | Check I2C wiring (SDA/SCL), verify power |
| "Lost power" warning | Install or replace CR2032 battery |
| Time resets on power cycle | Battery dead or not installed |
| Large time jumps | Poor I2C connection, check wires |
| Temperature seems wrong | DS3231 reads its own temp, not ambient |
| Excessive drift | Normal for first hour after power-on |

---

## 6. Sensors Test

### Purpose
Verify DHT22 temperature/humidity sensors and MQ135 air quality sensor provide accurate readings.

### Components Needed
- Arduino Uno R4 WiFi
- 2× DHT22 sensors (AM2302)
- 1× MQ135 air quality sensor
- 6 jumper wires
- Breadboard (recommended)

### Wiring Diagram

```
DHT22 Indoor (Pin 5):
  Pin 1 (VCC) → 5V
  Pin 2 (Data) → Arduino Pin 5
  Pin 3 (NC) → Not connected
  Pin 4 (GND) → GND

DHT22 Outdoor (Pin 6):
  Pin 1 (VCC) → 5V
  Pin 2 (Data) → Arduino Pin 6
  Pin 3 (NC) → Not connected
  Pin 4 (GND) → GND

MQ135 Air Quality (A0):
  VCC → 5V
  GND → GND
  AO (Analog Out) → Arduino A0
  DO (Digital Out) → Not used

Pull-up Resistors (if using 3-pin DHT22 modules):
  Usually built into module
  If separate DHT22 chip: Add 10kΩ resistor between Data and VCC
```

**DHT22 Module Types:**
- **4-pin module:** Usually has built-in pull-up resistor
- **3-pin breakout:** May need external 10kΩ pull-up
- **Bare sensor:** Requires 10kΩ pull-up resistor

**MQ135 Burn-in:**
MQ135 requires 24-48 hour burn-in period for stable readings:
- First use: Leave powered for 24+ hours
- Readings will stabilize after burn-in
- Normal for early readings to fluctuate

### Test Procedure

1. **Connect sensors**
   - DHT22 Indoor → Pin 5
   - DHT22 Outdoor → Pin 6
   - MQ135 → A0
   - All VCC → 5V, all GND → GND

2. **Wait for sensor stabilization**
   - DHT22: 2 seconds after power-on
   - MQ135: 20-30 seconds warm-up, 24-48 hours burn-in for accuracy

3. **Upload test sketch**
   ```
   File → Open → test_sensors.ino
   Sketch → Upload
   Tools → Serial Monitor (115200 baud)
   ```

4. **Observe continuous readings**
   New reading every 3 seconds (DHT22 minimum interval)

### Reading Display Format

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
├──────────────────────────────────────────────────────────────────────┤
│ Total Readings:  10  │  Errors - Indoor:  0  Outdoor:  0  AirQual:  0│
└──────────────────────────────────────────────────────────────────────┘
```

### Values Explained

**Temperature:**
- Direct reading from DHT22
- Range: -40°C to 80°C
- Accuracy: ±0.5°C

**Humidity:**
- Direct reading from DHT22
- Range: 0% to 100% RH
- Accuracy: ±2% RH

**Heat Index:**
- Computed perceived temperature
- Combines temperature and humidity
- Used in hot, humid conditions
- Formula: Steadman equation

**Dew Point:**
- Temperature at which water condenses
- Calculated using Magnus-Tetens formula
- Indicates absolute moisture level

**Humidex:**
- Canadian comfort index
- Temperature + humidity effect
- Masterton-Richardson equation

**Air Quality (MQ135):**
- Raw ADC: 0-1023 (analog reading)
- Estimated AQI: 0-500 (scaled value)
- AQI scale:
  - 0-50: Good
  - 51-100: Moderate
  - 101-150: Unhealthy for sensitive
  - 151-200: Unhealthy
  - 201-300: Very unhealthy
  - 301-500: Hazardous

### Status Report (Every 30s)

```
================= STATUS REPORT =================
Uptime: 90 seconds
Indoor sensor success rate: 100.0%
Outdoor sensor success rate: 100.0%
Air quality sensor success rate: 100.0%
✅ All sensors working perfectly!
================================================
```

### Testing Tips

**To see temperature changes:**
- Breathe on DHT22 sensor (raises temp and humidity)
- Use hair dryer on LOW heat from 30cm distance
- Place sensor near hot coffee cup

**To see humidity changes:**
- Breathe on sensor (raises humidity)
- Spray water mist near sensor (NOT on sensor!)

**To see air quality changes:**
- MQ135 detects: CO2, ammonia, benzene, smoke, alcohol
- Breathe on sensor (CO2 increases)
- Use alcohol-based hand sanitizer nearby
- Light incense (smoke detection)

### Success Criteria
- ✅ Both DHT22 sensors provide readings
- ✅ Temperature values reasonable (room temp ±20°C)
- ✅ Humidity values reasonable (30-80% typical indoor)
- ✅ Heat index close to temperature (±3°C)
- ✅ Dew point below temperature
- ✅ MQ135 provides stable ADC readings
- ✅ Success rate > 90% for all sensors
- ✅ Calculated values are plausible
- ✅ Readings change when conditions change

### Reasonable Value Ranges

**Indoor/Outdoor:**
- Temperature: 10-35°C (typical indoor)
- Humidity: 30-70% (comfortable range)
- Heat Index: Temperature to +10°C
- Dew Point: 5-20°C (temperate climate)
- Humidex: 20-35 (comfortable range)

**Air Quality:**
- Clean indoor air: 100-300 ADC
- Normal conditions: 300-500 ADC
- Poor ventilation: 500-700 ADC
- Smoke/contamination: 700+ ADC

### Troubleshooting
| Problem | Solution |
|---------|----------|
| DHT22 reads ERROR | Check wiring, verify 5V power, add pull-up resistor |
| Temperature seems high | Normal - sensor self-heating, allow 5min stabilization |
| Humidity reads 0% or 100% | Sensor damaged or connection issue |
| MQ135 reads max (1023) | Normal during warm-up, wait 30s-1min |
| MQ135 doesn't change | Requires 24-48h burn-in for sensitivity |
| Indoor/outdoor same values | Normal if sensors in same room |
| Frequent read errors | DHT22 minimum 2s between reads, sketch handles this |

---

## Wiring Diagrams

### Complete System Wiring

```
Arduino Uno R4 WiFi Pinout:

Digital Pins:
  Pin 2  → Button 1
  Pin 3  → Button 2
  Pin 5  → DHT22 Indoor (Data)
  Pin 6  → DHT22 Outdoor (Data)
  Pin 9  → 60 LED strip (Data)
  Pin 10 → 12 LED ring (Data)
  Pin 11 → 10 LED bar (Data)

Analog Pins:
  A0 → MQ135 (Analog Out)

I2C Pins (on R4):
  A4 (SDA) → LCD SDA, RTC SDA
  A5 (SCL) → LCD SCL, RTC SCL

Power:
  5V → All component VCC (from external supply for LEDs/LCD)
  GND → Common ground (all components)
  Vin → Optional 7-12V input (if using barrel jack)
```

### Power Distribution

```
External 5V/5A Supply:
  +5V ──┬── LED Strips VCC (high current)
        ├── LCD VCC (80-150mA)
        ├── Sensors VCC (low current)
        ├── RTC VCC (low current)
        └── Arduino 5V pin (if needed)

  GND ──┴── Common ground (all components + Arduino)

Note: Total current draw can reach 5A with all LEDs at full brightness
```

---

## Power Requirements

### Component Current Draw

| Component | Typical Current | Peak Current | Notes |
|-----------|----------------|--------------|-------|
| Arduino Uno R4 WiFi | 50-100mA | 200mA | Higher with WiFi active |
| 60 LED strip | 1.2A (average) | 3.6A | At full white brightness |
| 12 LED ring | 240mA (average) | 720mA | At full white brightness |
| 10 LED bar | 200mA (average) | 600mA | At full white brightness |
| LCD 20×4 | 80mA | 150mA | With backlight |
| DHT22 (each) | 1-2mA | 15mA | During reading |
| MQ135 | 150mA | 180mA | During warm-up |
| DS3231 RTC | 200μA | 300μA | Very low power |

### Total System Requirements

**Normal Operation** (typical project colors and brightness):
- Arduino: 100mA
- LEDs (30% brightness, mixed colors): 1.5A
- LCD: 100mA
- Sensors: 200mA
- **Total: ~2A**

**Worst Case** (all LEDs full white brightness):
- Arduino: 200mA
- LEDs (100% white): 5A
- LCD: 150mA
- Sensors: 200mA
- **Total: ~5.5A**

### Recommended Power Supplies

**For Testing:**
- 5V/5A power supply (minimum)
- Quality USB power adapter (5V/2.4A) for non-LED tests
- Bench power supply (variable, current limiting)

**For Final Project:**
- 5V/5-6A power supply
- Wall adapter with proper ratings
- Short, thick wires (18-22 AWG)
- Common ground essential

---

## Next Steps

After all hardware tests pass:

1. ✅ **Document your setup**
   - Take photos of wiring
   - Note any component-specific issues
   - Record successful pin configurations

2. ✅ **Proceed to software testing**
   - See [software_tests.md](./software_tests.md)
   - Test WiFi connectivity
   - Test NTP synchronization
   - Test web interface

3. ✅ **System integration**
   - Upload main firmware
   - Configure settings
   - Final system validation

---

**Author:** F. Baillon  
**Version:** 1.0.0  
**Last Updated:** January 2025