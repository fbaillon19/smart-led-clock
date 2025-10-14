# Troubleshooting Guide - Smart LED Clock 🔧

> Common issues, solutions, and debugging techniques

This guide helps you diagnose and fix common problems with the Smart LED Clock project.

---

## 📋 Table of Contents

- [Quick Diagnosis](#quick-diagnosis)
- [Hardware Issues](#hardware-issues)
- [Software Issues](#software-issues)
- [Network Issues](#network-issues)
- [Web Interface Issues](#web-interface-issues)
- [Sensor Issues](#sensor-issues)
- [Advanced Debugging](#advanced-debugging)

---

## Quick Diagnosis

### Symptom-Based Problem Finder

**Nothing works at all:**
→ [Power Supply Issues](#power-supply-issues)

**Arduino not responding:**
→ [Arduino Board Issues](#arduino-board-issues)

**LEDs not lighting:**
→ [LED Strip Issues](#led-strip-issues)

**LCD blank or garbled:**
→ [LCD Display Issues](#lcd-display-issues)

**Buttons don't respond:**
→ [Button Issues](#button-issues)

**Time is wrong:**
→ [RTC Issues](#rtc-issues)

**Temperature readings error:**
→ [DHT22 Sensor Issues](#dht22-sensor-issues)

**Can't connect to WiFi:**
→ [WiFi Connection Issues](#wifi-connection-issues)

**Web page won't load:**
→ [Web Server Issues](#web-server-issues)

**System crashes/resets:**
→ [Stability Issues](#stability-issues)

---

## Hardware Issues

### Power Supply Issues

#### Problem: Nothing turns on, no lights, no display

**Diagnostic Steps:**
1. Check power supply is plugged in and turned on
2. Verify power supply voltage with multimeter (should be ~5V)
3. Check power connections to Arduino and components
4. Verify common ground connections

**Solutions:**
```
Issue: Power supply unplugged
Fix: Plug in power supply

Issue: Power supply too weak (< 5V/3A)
Fix: Use 5V/5A power supply minimum

Issue: Fuse blown in power supply
Fix: Replace power supply

Issue: Loose connections
Fix: Secure all power wiring, use proper connectors

Issue: Voltage drop in wires
Fix: Use thicker wires (18-22 AWG), keep runs short
```

**Expected Voltages:**
- Power supply output: 5.0V ± 0.25V
- At Arduino 5V pin: 4.75V - 5.25V
- At LED VCC: 4.75V minimum
- At sensor VCC: 4.5V - 5.5V

---

#### Problem: Arduino powers on but components don't work

**Diagnostic Steps:**
1. Measure voltage at Arduino 5V pin
2. Measure voltage at component VCC pins
3. Check current draw with multimeter
4. Verify ground continuity

**Solutions:**
```
Issue: USB power only (insufficient for LEDs/LCD)
Fix: Connect external 5V power supply

Issue: Arduino can't supply enough current
Fix: Power LEDs directly from external supply, not through Arduino

Issue: Bad ground connection
Fix: Ensure all GNDs connected together (common ground)

Issue: Voltage regulator overheating
Fix: Reduce load, improve heat dissipation

Issue: Polarity reversed
Fix: Check +/- connections, correct any reversed polarity
```

---

### Arduino Board Issues

#### Problem: Arduino not detected by computer

**Diagnostic Steps:**
1. Try different USB cable
2. Try different USB port on computer
3. Check Device Manager (Windows) or System Report (macOS)
4. Test Arduino with simple blink sketch

**Solutions:**
```
Issue: Bad USB cable (data lines broken)
Fix: Use known-good USB cable with data capability

Issue: USB port not working
Fix: Try different USB port, preferably USB 3.0

Issue: Driver not installed (Windows)
Fix: Install Arduino driver from arduino.cc

Issue: Board in bootloader mode stuck
Fix: Double-tap reset button to enter bootloader

Issue: Damaged USB port on Arduino
Fix: May require board replacement or serial programming

Issue: Wrong board selected in Arduino IDE
Fix: Tools → Board → Arduino Uno R4 WiFi
```

---

#### Problem: Sketch won't upload

**Diagnostic Steps:**
1. Verify correct board selected in Arduino IDE
2. Verify correct COM port selected
3. Check for serial monitor open (close it)
4. Try upload with reset button press

**Solutions:**
```
Issue: Wrong board selected
Fix: Tools → Board → Arduino Uno R4 WiFi

Issue: Wrong port selected  
Fix: Tools → Port → [select correct COM/ttyUSB port]

Issue: Serial monitor open
Fix: Close serial monitor before uploading

Issue: Sketch too large
Fix: Reduce code size, remove debug strings, optimize

Issue: Compilation errors
Fix: Check for missing libraries, syntax errors

Issue: Bootloader corruption
Fix: Re-burn bootloader using ISP programmer
```

---

### LED Strip Issues

#### Problem: LEDs don't light up at all

**Diagnostic Steps:**
1. Check power supply to LED strips (5V present?)
2. Verify data line connections
3. Test with simple NeoPixel example sketch
4. Check LED strip orientation (data flow direction)

**Solutions:**
```
Issue: No power to LED strips
Fix: Connect 5V and GND to LED strip VCC/GND

Issue: Insufficient power (USB only)
Fix: MUST use external 5V/5A power supply

Issue: Data line not connected
Fix: Connect Arduino pin to LED "Data In" (DIN)

Issue: LED strip backwards
Fix: Connect to "Data In" end, not "Data Out" end

Issue: Wrong pin in code
Fix: Verify pin numbers match wiring (9, 10, 11)

Issue: Dead first LED
Fix: Cut off first LED, solder to next LED

Issue: Wrong LED type in code
Fix: Check if strip is NEO_GRB or NEO_RGB
```

---

#### Problem: LEDs are dim or wrong colors

**Diagnostic Steps:**
1. Measure voltage at LED strips (should be 5V)
2. Check current draw with multimeter
3. Test brightness settings in code
4. Verify color order (RGB vs GRB)

**Solutions:**
```
Issue: Voltage drop (insufficient power)
Fix: Use thicker wires, shorter runs, higher capacity supply

Issue: USB power only
Fix: Connect external 5V/5A supply directly to LED strips

Issue: Brightness set too low
Fix: Increase brightness in code or web interface

Issue: Color order wrong (GRB vs RGB)
Fix: Change NEO_GRB to NEO_RGB (or vice versa) in code

Issue: Mixed LED types
Fix: Use same type/brand LEDs throughout

Issue: Overheating
Fix: Reduce brightness, improve air circulation

Issue: Long wire runs causing signal degradation
Fix: Add 330Ω resistor on data line, use shorter wires
```

---

#### Problem: Some LEDs work, some don't

**Diagnostic Steps:**
1. Identify which LEDs aren't working
2. Check if dead LEDs are in sequence
3. Test continuity through data line
4. Measure voltage along strip

**Solutions:**
```
Issue: Data line broken at specific point
Fix: Bypass broken section or replace strip

Issue: Dead LEDs in middle of strip
Fix: Cut out dead LEDs, reconnect data line

Issue: Poor solder joint
Fix: Reflow solder joints, use proper soldering technique

Issue: Voltage drop along strip
Fix: Inject power at multiple points along strip

Issue: Capacitance on data line
Fix: Add 100µF capacitor across power at beginning

Issue: Signal integrity
Fix: Add 330Ω resistor between Arduino and first LED
```

---

### LCD Display Issues

#### Problem: LCD shows nothing (blank screen)

**Diagnostic Steps:**
1. Check backlight (dim glow visible?)
2. Adjust contrast potentiometer
3. Verify I2C connections (SDA/SCL)
4. Run I2C scanner to detect device

**Solutions:**
```
Issue: Backlight off
Fix: Check backlight jumper on module back, verify power

Issue: Contrast too low
Fix: Adjust blue potentiometer on back with screwdriver

Issue: No power
Fix: Connect VCC to 5V, GND to GND

Issue: I2C not connected
Fix: SDA → A4, SCL → A5, verify wiring

Issue: Wrong I2C address
Fix: Run I2C scanner, update address in code (0x27 or 0x3F)

Issue: LCD module damaged
Fix: Test with another LCD or replace module
```

---

#### Problem: LCD shows garbled text or blocks

**Diagnostic Steps:**
1. Adjust contrast potentiometer
2. Check I2C address in code
3. Verify 5V power supply voltage
4. Test with simple LCD example sketch

**Solutions:**
```
Issue: Contrast adjustment needed
Fix: Turn potentiometer on back until text clear

Issue: Wrong I2C address
Fix: Change LCD_I2C_ADDRESS from 0x27 to 0x3F (or vice versa)

Issue: Insufficient power
Fix: Use external 5V supply, not USB only

Issue: I2C communication errors
Fix: Add 4.7kΩ pull-up resistors on SDA/SCL

Issue: Interference on I2C bus
Fix: Keep I2C wires short, away from power wires

Issue: Multiple devices same address
Fix: Check no other I2C device uses same address
```

---

#### Problem: LCD flickers or resets randomly

**Diagnostic Steps:**
1. Check power supply stability
2. Measure voltage under load
3. Look for voltage drops
4. Check for loose connections

**Solutions:**
```
Issue: Insufficient power capacity
Fix: Use larger power supply (5V/3A minimum)

Issue: Long power wire runs
Fix: Use thicker wires (18-22 AWG)

Issue: Loose connections
Fix: Secure all power and I2C connections

Issue: Ground loops
Fix: Ensure single-point grounding

Issue: Excessive load on I2C bus
Fix: Reduce I2C clock speed if needed

Issue: Code updating too frequently
Fix: Reduce LCD update rate, use anti-flicker techniques
```

---

### Button Issues

#### Problem: Button presses not detected

**Diagnostic Steps:**
1. Verify button wiring (pin to GND)
2. Test button with multimeter (continuity)
3. Check button orientation
4. Test with simple button example

**Solutions:**
```
Issue: Button not connected
Fix: Connect one leg to Arduino pin, other to GND

Issue: Button wired to wrong pin
Fix: Verify Pin 2 (Button 1) and Pin 3 (Button 2)

Issue: Button damaged/stuck
Fix: Replace button, check for debris

Issue: No pull-up resistor (if not using INPUT_PULLUP)
Fix: Use pinMode(pin, INPUT_PULLUP) or add 10kΩ resistor

Issue: Pin configured as OUTPUT
Fix: Set pin as INPUT_PULLUP in setup()

Issue: Button pressed but backwards in circuit
Fix: Buttons are non-polarized, should work either way
```

---

#### Problem: Button triggers multiple times (bouncing)

**Diagnostic Steps:**
1. Check debounce code implementation
2. Test with different button
3. Monitor serial output for bounces

**Solutions:**
```
Issue: No debounce implemented
Fix: Use OneButton library (already in test code)

Issue: Bad quality button (excessive bounce)
Fix: Replace with better quality tactile switch

Issue: Debounce time too short
Fix: Increase debounce time from 50ms to 100ms

Issue: Electrical noise
Fix: Add 0.1µF capacitor across button terminals

Issue: Long wires acting as antenna
Fix: Keep button wires short (<15cm)

Issue: Button mechanically worn
Fix: Replace button with new one
```

---

### RTC Issues

#### Problem: RTC not detected on I2C

**Diagnostic Steps:**
1. Run I2C scanner
2. Verify connections (SDA/SCL)
3. Check 5V power to module
4. Test with simple RTC example

**Solutions:**
```
Issue: I2C not connected
Fix: SDA → A4, SCL → A5

Issue: No power to RTC
Fix: VCC → 5V, GND → GND

Issue: Wrong I2C address expected
Fix: DS3231 uses 0x68 (fixed)

Issue: I2C pull-up resistors missing
Fix: Most modules have built-in, add 4.7kΩ if not

Issue: Damaged RTC module
Fix: Test with multimeter, replace if needed

Issue: I2C bus conflict
Fix: Disconnect other I2C devices, test individually
```

---

#### Problem: Time keeps resetting

**Diagnostic Steps:**
1. Check if battery installed (CR2032)
2. Measure battery voltage (should be ~3V)
3. Check "lostPower()" status in serial
4. Verify time setting code works

**Solutions:**
```
Issue: No battery installed
Fix: Install CR2032 battery in holder

Issue: Battery dead
Fix: Replace with new CR2032 (3V)

Issue: Battery holder poor contact
Fix: Clean contacts, ensure battery seated properly

Issue: Code calls adjust() every boot
Fix: Only call adjust() when lostPower() is true

Issue: Power supply unstable
Fix: Use stable 5V supply, add capacitor

Issue: VBAT pin not connected
Fix: Check DS3231 module schematic, verify VBAT
```

---

#### Problem: Time drifts (inaccurate)

**Diagnostic Steps:**
1. Monitor drift over 24 hours
2. Check temperature readings
3. Verify NTP sync working
4. Compare to accurate time source

**Solutions:**
```
Issue: NTP not syncing
Fix: Check WiFi, verify NTP server accessible

Issue: Normal DS3231 drift (±2ppm)
Fix: This equals ±1 minute/year - acceptable

Issue: Temperature extremes
Fix: Keep RTC at stable room temperature

Issue: Fake/counterfeit DS3231
Fix: Purchase from reputable supplier

Issue: Crystal damaged
Fix: Replace RTC module

Issue: Timezone offset wrong
Fix: Verify TIME_ZONE_OFFSET in config.h
```

---

### DHT22 Sensor Issues

#### Problem: Sensor returns "ERROR" or NaN

**Diagnostic Steps:**
1. Check sensor wiring (VCC, GND, Data)
2. Verify 2-second minimum between reads
3. Test with simple DHT example
4. Check pull-up resistor (10kΩ)

**Solutions:**
```
Issue: Wiring incorrect
Fix: VCC → 5V, GND → GND, Data → Arduino pin

Issue: Reading too frequently
Fix: Minimum 2 seconds between reads (built into library)

Issue: No pull-up resistor
Fix: Add 10kΩ resistor between Data and VCC (some modules have built-in)

Issue: Data wire too long (>20cm)
Fix: Keep data wire short, use shielded cable

Issue: Power supply noisy
Fix: Add 0.1µF capacitor across sensor VCC/GND

Issue: Sensor damaged
Fix: DHT22 sensitive to overvoltage - replace if damaged

Issue: Timing conflicts in code
Fix: Ensure no blocking delays, use non-blocking code
```

---

#### Problem: Temperature readings seem wrong

**Diagnostic Steps:**
1. Compare to known-good thermometer
2. Check sensor location (direct sunlight? heat source?)
3. Wait 5 minutes for stabilization
4. Verify units (Celsius vs Fahrenheit)

**Solutions:**
```
Issue: Self-heating effect
Fix: Normal - sensor reads 1-2°C above ambient when reading

Issue: Direct sunlight or heat source
Fix: Shield sensor from direct sun, heat sources

Issue: Poor ventilation
Fix: Ensure air can circulate around sensor

Issue: Units wrong in code
Fix: readTemperature() for °C, readTemperature(true) for °F

Issue: Calibration offset
Fix: Add/subtract calibration constant if needed

Issue: Sensor mounted on PCB with heat sources
Fix: Mount sensor remotely on wires
```

---

### MQ135 Air Quality Sensor Issues

#### Problem: Air quality readings always max (1023) or min (0)

**Diagnostic Steps:**
1. Check sensor wiring (VCC, GND, AO)
2. Verify sensor has been powered for warm-up (30s minimum)
3. Check if sensor needs burn-in (24-48 hours)
4. Measure voltage at AO pin

**Solutions:**
```
Issue: Not warmed up
Fix: Wait 30 seconds after power on

Issue: Not burned in
Fix: Leave powered continuously for 24-48 hours

Issue: AO not connected
Fix: Connect AO to Arduino A0

Issue: Using DO instead of AO
Fix: Use analog out (AO), not digital out (DO)

Issue: Sensor saturated
Fix: Ventilate area, sensor detecting high pollution

Issue: Sensor damaged
Fix: MQ sensors delicate - replace if damaged

Issue: Wrong sensor type
Fix: Verify sensor is MQ135, not MQ2/MQ7/etc
```

---

## Software Issues

### Compilation Errors

#### Problem: "Library not found" errors

**Solution:**
```
1. Open Arduino IDE
2. Sketch → Include Library → Manage Libraries
3. Search for missing library
4. Click Install
5. Restart Arduino IDE
6. Try compiling again
```

**Required Libraries:**
- Adafruit NeoPixel
- LiquidCrystal I2C
- RTClib (by Adafruit)
- DHT sensor library (by Adafruit)
- OneButton
- WiFiS3 (built-in for Uno R4)

---

#### Problem: "Sketch too large" error

**Solutions:**
```
Issue: Too many features/debug strings
Fix: Remove Serial.print() debug statements
Fix: Use F() macro for string literals: Serial.println(F("text"));
Fix: Disable unused features in config.h
Fix: Optimize code, remove duplicate code

Issue: Too many libraries
Fix: Remove unused #include statements

Issue: Wrong board selected
Fix: Ensure Arduino Uno R4 WiFi selected (has more memory than Uno)

Issue: Flash memory genuinely full
Fix: Simplify project or use larger board
```

---

### Upload Errors

#### Problem: "avrdude: stk500_recv(): programmer is not responding"

**Solutions:**
```
1. Close Serial Monitor
2. Disconnect all components except USB
3. Press reset button on Arduino
4. Try upload immediately after reset
5. Try different USB port
6. Try different USB cable
7. Verify correct board and port selected
```

---

### Runtime Errors

#### Problem: Arduino resets repeatedly

**Diagnostic Steps:**
1. Monitor serial output for reset cause
2. Check power supply capacity
3. Look for watchdog timer triggers
4. Check for infinite loops or stack overflow

**Solutions:**
```
Issue: Insufficient power (brownout)
Fix: Use adequate power supply (5V/5A)

Issue: Watchdog timer timeout
Fix: Disable watchdog or increase timeout

Issue: Stack overflow
Fix: Reduce local variable sizes, use dynamic allocation carefully

Issue: Infinite loop in code
Fix: Add timeout conditions, use non-blocking code

Issue: Memory corruption
Fix: Check array bounds, buffer overflows

Issue: Short circuit
Fix: Check for wiring shorts, especially on LEDs/LCD
```

---

## Network Issues

### WiFi Connection Issues

#### Problem: "WiFi connection failed"

**Diagnostic Steps:**
1. Verify SSID and password in secrets.h
2. Check if network is 2.4GHz (not 5GHz)
3. Verify WPA2 security (WPA3 not supported)
4. Check router settings

**Solutions:**
```
Issue: Wrong SSID or password
Fix: Double-check credentials in secrets.cpp

Issue: 5GHz network
Fix: Arduino R4 only supports 2.4GHz - configure router for 2.4GHz

Issue: Hidden SSID
Fix: Make SSID visible or manually specify in code

Issue: MAC filtering enabled
Fix: Add Arduino MAC address to router whitelist

Issue: WPA3 encryption
Fix: Enable WPA2 compatibility mode on router

Issue: Special characters in password
Fix: Ensure password properly escaped in code

Issue: Too far from router
Fix: Move closer or use WiFi repeater/extender

Issue: Network congested
Fix: Change WiFi channel in router settings
```

---

#### Problem: WiFi connects then disconnects

**Diagnostic Steps:**
1. Check signal strength (should be > -80 dBm)
2. Monitor for interference
3. Check router logs
4. Test with different location

**Solutions:**
```
Issue: Weak signal
Fix: Move Arduino closer to router, improve antenna

Issue: Channel interference
Fix: Change WiFi channel to less congested one (1, 6, or 11)

Issue: Router DHCP table full
Fix: Increase DHCP pool size or assign static IP

Issue: Router dropping inactive clients
Fix: Adjust router timeout settings

Issue: Power supply insufficient
Fix: WiFi uses more power - use adequate supply

Issue: Memory leak in code
Fix: Monitor free memory, check for leaks

Issue: Overheating
Fix: Improve ventilation, reduce workload
```

---

### NTP Synchronization Issues

#### Problem: NTP sync fails

**Diagnostic Steps:**
1. Verify internet connectivity
2. Check firewall settings (UDP port 123)
3. Try different NTP server
4. Check DNS resolution

**Solutions:**
```
Issue: No internet access
Fix: Verify router connected to internet

Issue: Firewall blocking UDP port 123
Fix: Allow UDP port 123 outbound

Issue: NTP server down
Fix: Change NTP server in code:
  - pool.ntp.org (default)
  - time.nist.gov
  - time.google.com

Issue: DNS resolution failing
Fix: Use IP address instead of hostname

Issue: Timeout too short
Fix: Increase NTP timeout in code

Issue: Incorrect timezone offset
Fix: Verify TIME_ZONE_OFFSET matches your location
```

---

## Web Interface Issues

### Web Server Issues

#### Problem: Can't access web interface

**Diagnostic Steps:**
1. Verify Arduino has IP address (check serial)
2. Ping Arduino IP from computer
3. Verify on same network
4. Check firewall on computer

**Solutions:**
```
Issue: No IP address assigned
Fix: Check WiFi connection, verify DHCP enabled

Issue: Wrong IP address
Fix: Check serial monitor for correct IP

Issue: Different network/subnet
Fix: Connect computer to same WiFi network as Arduino

Issue: Firewall blocking port 80
Fix: Allow port 80 in computer firewall

Issue: Arduino not responding to ping
Fix: Check power, verify WiFi connected

Issue: Wrong URL format
Fix: Use http://192.168.1.150 (not https://)
```

---

#### Problem: Web page loads partially or slowly

**Diagnostic Steps:**
1. Check page size and memory usage
2. Monitor serial for errors
3. Try different browser
4. Check network latency

**Solutions:**
```
Issue: Insufficient memory on Arduino
Fix: Reduce page complexity, optimize HTML/CSS

Issue: Slow WiFi connection
Fix: Improve signal strength, reduce interference

Issue: Too many AJAX requests
Fix: Reduce update frequency

Issue: Large images/resources
Fix: Optimize images, use compressed resources

Issue: Browser caching disabled
Fix: Enable caching, set proper cache headers

Issue: Serial output slowing down web server
Fix: Reduce debug print statements
```

---

### Dashboard Issues

#### Problem: Sensor values show "N/A" or don't update

**Diagnostic Steps:**
1. Check if sensors working (serial output)
2. Verify API endpoint returns data
3. Check browser console for JavaScript errors
4. Monitor network tab in browser dev tools

**Solutions:**
```
Issue: Sensors not providing data
Fix: Check sensor connections, verify sensor test passes

Issue: API endpoint failing
Fix: Test endpoint directly: http://[IP]/api/status

Issue: JavaScript errors
Fix: Check browser console (F12), fix JavaScript

Issue: Auto-refresh not working
Fix: Verify setInterval() code, check for JavaScript errors

Issue: Incorrect JSON parsing
Fix: Verify API returns valid JSON

Issue: CORS issues (if testing from file://)
Fix: Access via http://[IP], not file://
```

---

## Advanced Debugging

### Serial Debugging

**Enable verbose output:**
```cpp
// In config.h
#define DEBUG_MODE true

// In code
#ifdef DEBUG_MODE
  Serial.println("Debug: Variable value = " + String(value));
#endif
```

**Serial output tips:**
- Use meaningful messages: "WiFi connected" not just "OK"
- Print variable values: Serial.print("Temp: "); Serial.println(temp);
- Use timestamps: Serial.print(millis()); Serial.print(": Message");
- Flush buffer when needed: Serial.flush();

---

### Memory Debugging

**Check free memory (AVR):**
```cpp
int freeRAM() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void loop() {
  Serial.print("Free RAM: ");
  Serial.println(freeRAM());
}
```

**Memory leak detection:**
1. Monitor free RAM over time
2. Should remain stable
3. Decreasing = potential memory leak
4. Check for missing free() calls
5. Avoid String class (use char arrays)

---

### I2C Debugging

**I2C Scanner Sketch:**
```cpp
#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial.println("I2C Scanner");
}

void loop() {
  byte error, address;
  int nDevices = 0;

  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Device at 0x");
      if (address<16) Serial.print("0");
      Serial.println(address,HEX);
      nDevices++;
    }
  }
  
  if (nDevices == 0) {
    Serial.println("No I2C devices found");
  }
  
  delay(5000);
}
```

**Common I2C addresses:**
- 0x27 or 0x3F: LCD
- 0x68: DS3231 RTC
- 0x76 or 0x77: BME280 (if used)

---

### Logic Analyzer

For complex timing issues:
1. Use cheap logic analyzer ($10-20)
2. Connect to I2C lines (SDA/SCL)
3. Use software like Pulseview
4. Analyze I2C transactions
5. Verify timing and ACK/NACK

---

### Multimeter Usage

**Voltage checks:**
- Power supply: 5.0V ± 0.25V
- Arduino 5V pin: 4.75-5.25V
- Sensor VCC: 4.5-5.5V

**Continuity checks:**
- GND connections: should beep
- Data lines: no shorts to VCC/GND
- Switch/button: beeps when pressed

**Current measurement:**
- Total system: should match expectations
- LED strip: high current (1-5A)
- Sensors: low current (<20mA each)

---

## Getting More Help

If issues persist after trying solutions here:

1. **Check documentation:**
   - [Hardware Tests](./hardware_tests.md)
   - [Software Tests](./software_tests.md)
   - [Main Documentation](../docs/)

2. **Search online:**
   - Arduino forum: forum.arduino.cc
   - Stack Overflow: stackoverflow.com
   - Component-specific forums

3. **Ask for help:**
   - Provide detailed description
   - Include error messages
   - Share relevant code snippets
   - Include photos of wiring
   - Describe what you've already tried

4. **Create GitHub issue:**
   - Use issue template
   - Include system information
   - Attach serial monitor output
   - Include test results

---

## Preventive Maintenance

### Regular Checks (Monthly)

- ✅ Verify all connections secure
- ✅ Check for dust accumulation
- ✅ Test sensor accuracy
- ✅ Verify RTC battery voltage
- ✅ Check power supply voltage
- ✅ Update firmware if available

### Cleaning

- Use compressed air for dust
- Clean LCD screen with microfiber cloth
- Check for corrosion on contacts
- Ensure good ventilation

### Battery Replacement

- CR2032 in DS3231: Every 8-10 years
- Check battery voltage: 2.7-3.3V normal
- Replace if voltage < 2.5V
- Set time after battery replacement

---

**Remember:**
- Always disconnect power before working on circuits
- Double-check connections before applying power
- Document changes you make
- Keep backups of working configurations

**Author:** F. Baillon  
**Version:** 1.0.0  
**Last Updated:** January 2025