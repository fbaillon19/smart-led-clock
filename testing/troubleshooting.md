# Troubleshooting Guide

This guide covers common issues encountered during Smart LED Clock assembly and operation, along with step-by-step solutions.

## 🔧 General Troubleshooting Process

1. **Identify the Problem**: What exactly isn't working?
2. **Check Power**: Ensure stable 5V supply with adequate current
3. **Verify Connections**: Look for loose wires, cold solder joints
4. **Test Components**: Use individual test codes to isolate issues
5. **Check Software**: Verify correct libraries and configuration
6. **Monitor Serial**: Enable debug mode for detailed information

---

## ⚡ Power Supply Issues

### Symptom: Arduino won't boot or resets randomly
**Common Causes:**
- Insufficient power supply capacity
- Voltage drop under load
- Poor quality power supply

**Solutions:**
1. **Check power supply specifications**
   ```
   Required: 5V, minimum 3A (5A recommended)
   Measure actual voltage: should be 4.8-5.2V
   ```

2. **Test with LED strips disconnected**
   - If Arduino works without LEDs, power supply is insufficient
   - LED strips can draw up to 3A at full brightness

3. **Add capacitors** (advanced)
   - 1000µF electrolytic capacitor near power input
   - 100µF capacitor near Arduino power pins

### Symptom: LEDs dim or flicker
**Causes:** Voltage drop, insufficient current capacity

**Solutions:**
- Use thicker wires for power distribution (16-18 AWG)
- Add power injection points for long LED strips
- Reduce LED brightness in software as temporary fix

---

## 💡 LED Strip Issues

### Symptom: LEDs don't light up at all
**Diagnostic Steps:**
1. **Check power connections**
   ```
   Red wire (5V) → Arduino 5V
   Black wire (GND) → Arduino GND
   Data wire → Correct digital pin
   ```

2. **Verify data pin assignment**
   ```cpp
   // In config.h, check these match your wiring:
   #define PIN_LEDS_MINUTE_SECOND  9
   #define PIN_LEDS_HOUR          10
   #define PIN_LEDS_AIR_QUALITY   11
   ```

3. **Test with simple code**
   - Upload `testing/test_codes/test_led/test_led.ino`
   - Should show rainbow pattern

**Common Fixes:**
- Reverse data wire direction (DOUT to DIN)
- Check for broken first LED (bypass and test)
- Verify LED strip type (WS2812B vs WS2811)

### Symptom: Some LEDs don't work or show wrong colors
**Causes:**
- Damaged LEDs in strip
- Signal degradation over distance
- Power supply voltage drop

**Solutions:**
1. **Identify problem location**
   - LEDs work before problem point
   - All LEDs after problem point fail

2. **Power injection** (for long strips)
   ```
   Add 5V/GND connection every 60 LEDs
   Use 18 AWG wire for power injection
   ```

3. **Signal level shifting** (advanced)
   - Add 74HCT245 level shifter for reliable data signal
   - Or use 330Ω resistor in data line

### Symptom: LEDs show wrong colors
**Quick Fixes:**
1. **Check color order in code**
   ```cpp
   // Try different color orders:
   NEO_GRB  // Most WS2812B strips
   NEO_RGB  // Some variations
   NEO_BRG  // Less common
   ```

2. **Test with known colors**
   ```cpp
   strip.setPixelColor(0, 255, 0, 0);   // Should be red
   strip.setPixelColor(1, 0, 255, 0);   // Should be green
   strip.setPixelColor(2, 0, 0, 255);   // Should be blue
   ```

---

## 📟 LCD Display Issues

### Symptom: LCD shows no text or garbled characters
**Diagnostic Steps:**
1. **I2C Address Scan**
   - Run I2C scanner code
   - Common addresses: 0x27, 0x3F, 0x20

2. **Check wiring**
   ```
   LCD → Arduino UNO R4
   SDA → A4 (or dedicated SDA pin)
   SCL → A5 (or dedicated SCL pin)  
   VCC → 5V
   GND → GND
   ```

3. **Test with simple code**
   - Upload `testing/test_codes/test_lcd/test_lcd.ino`
   - Should show test patterns

**Common Fixes:**
- Adjust I2C address in code to match scanner results
- Check for loose connections on I2C backpack
- Verify 5V power to LCD (not 3.3V)

### Symptom: LCD backlight works but no text
**Causes:** 
- Wrong I2C address
- Damaged LCD module
- I2C communication issues

**Solutions:**
1. **Try different I2C addresses**
   ```cpp
   // Common addresses to try:
   LiquidCrystal_I2C lcd(0x27, 20, 4);
   LiquidCrystal_I2C lcd(0x3F, 20, 4);
   LiquidCrystal_I2C lcd(0x20, 20, 4);
   ```

2. **Check contrast** (if adjustable)
   - Look for small potentiometer on I2C backpack
   - Turn slowly while observing display

### Symptom: Text appears but is unreadable
**Quick Fixes:**
- Adjust contrast potentiometer
- Check 5V power supply stability
- Try different I2C pull-up resistors (4.7kΩ)

---

## 🌡️ Temperature Sensor Issues

### Symptom: Temperature shows "NaN" or obviously wrong values
**Diagnostic Steps:**
1. **Check DHT22 wiring**
   ```
   DHT22 → Arduino
   Pin 1 (VCC) → 5V
   Pin 2 (Data) → Digital pin (D6 or D7)
   Pin 3 (NC) → Not connected
   Pin 4 (GND) → GND
   ```

2. **Test sensors individually**
   - Upload `testing/test_codes/test_sensors/test_sensors.ino`
   - Check serial monitor for readings

3. **Verify power supply**
   - DHT22 requires stable 3.3-5V
   - Check voltage at sensor pins with multimeter

**Common Fixes:**
- Add 10kΩ pull-up resistor on data line (usually not needed)
- Replace sensor if consistently failing
- Allow 2-3 seconds between readings
- Wait 2 seconds after power-up before first reading

### Symptom: One sensor works, other doesn't
**Likely Causes:**
- Wiring error on non-working sensor
- Damaged sensor
- Pin conflict in software

**Solutions:**
1. **Swap sensor connections**
   - If problem follows sensor: sensor is damaged
   - If problem stays with pin: wiring or software issue

2. **Check pin assignments**
   ```cpp
   #define PIN_DHT_INDOOR  6
   #define PIN_DHT_OUTDOOR 7
   ```

### Symptom: Readings are unstable or jump around
**Causes:** 
- Power supply noise
- Electromagnetic interference
- Damaged sensor

**Solutions:**
- Add 100µF capacitor near sensors
- Keep sensor wires away from LED strips
- Use shielded cable for outdoor sensor
- Filter readings in software (moving average)

---

## 🌐 WiFi Connection Issues

### Symptom: Cannot connect to WiFi network
**Diagnostic Steps:**
1. **Check credentials**
   ```cpp
   // In secret.h:
   const char* ssid = "YourWiFiName";     // Exact name
   const char* pass = "YourWiFiPassword"; // Case sensitive
   ```

2. **Test WiFi signal strength**
   - Move Arduino closer to router
   - Check for 2.4GHz vs 5GHz (Arduino only supports 2.4GHz)

3. **Monitor connection process**
   ```cpp
   Serial.print("Connecting to: ");
   Serial.println(ssid);
   // Enable debug output
   ```

**Common Fixes:**
- Verify WiFi credentials are exactly correct
- Check network security type (WPA2 supported)
- Restart router/modem
- Try different WiFi channel (1, 6, or 11)
- Check for MAC address filtering

### Symptom: Connects to WiFi but can't reach internet
**Solutions:**
- Check router's internet connection
- Verify DNS settings
- Try different NTP servers:
  ```cpp
  "pool.ntp.org"
  "time.nist.gov"  
  "time.google.com"
  ```

### Symptom: Connection drops frequently
**Causes:**
- Weak signal strength
- Power supply issues
- Router configuration

**Solutions:**
- Improve WiFi antenna positioning
- Use WiFi extender/repeater
- Implement connection retry logic
- Check for power supply voltage drops

---

## 🔘 Button Issues

### Symptom: Buttons don't respond or respond erratically
**Diagnostic Steps:**
1. **Check wiring**
   ```
   Button → Arduino
   One leg → Digital pin (D2 or D3)
   Other leg → GND
   ```

2. **Test with multimeter**
   - Should read 0Ω when pressed, infinite when released
   - Check for intermittent connections

3. **Use button test code**
   - Upload `testing/test_codes/test_buttons/test_buttons.ino`
   - Monitor serial output while pressing buttons

**Common Fixes:**
- Clean button contacts with contact cleaner
- Check for loose connections
- Verify INPUT_PULLUP mode in software
- Add debouncing delay in code

### Symptom: Multiple button presses registered
**Cause:** Button bounce (mechanical)

**Software Solution:**
```cpp
// Already implemented in OneButton library
button.setDebounceTicks(50);  // 50ms debounce
```

---

## 🕐 DS3231 RTC Issues

### Symptom: DS3231 not detected on I2C bus
**Diagnostic Steps:**
1. **Run I2C scanner** - DS3231 should appear at address 0x68
2. **Check wiring**
   ```
   DS3231 → Arduino UNO R4
   VCC → 5V (or 3.3V)
   GND → GND
   SDA → A4 (shared with LCD)
   SCL → A5 (shared with LCD)
   ```
3. **Test with DS3231 test code**
   - Upload `testing/test_codes/test_rtc/test_rtc.ino`
   - Monitor serial output for detection

**Common Fixes:**
- Verify I2C connections (SDA/SCL not swapped)
- Check power supply to DS3231 (3.3V or 5V both work)
- Ensure no short circuits on I2C bus
- Try different DS3231 module if suspected faulty

### Symptom: Time resets to 2000 or compile time on each boot
**Cause:** DS3231 losing power, no battery backup

**Solutions:**
1. **Install backup battery**
   - Insert CR2032 lithium battery in DS3231 module
   - Battery provides backup power during main power loss
   
2. **Check battery connection**
   - Ensure battery holder contacts are clean
   - Verify battery voltage (should be 3V)
   - Replace battery if old (8-10 year lifespan)

### Symptom: Time drifts significantly over days/weeks
**Expected Performance:** DS3231 should be accurate to ±2ppm (±1 minute per year)

**Diagnostic:**
1. **Monitor drift with test code**
   - Use `test_rtc.ino` to track drift over time
   - Compare with NTP time sync
   
2. **Check temperature compensation**
   - DS3231 has built-in temperature compensation
   - Extreme temperatures may affect accuracy

**Solutions:**
- Most DS3231 modules are very accurate out of box
- If drift is excessive (>1 minute/month), module may be faulty
- Consider aging offset adjustment (advanced feature)

### Symptom: Cannot set time or time doesn't update
**Causes:**
- Communication error with DS3231
- Faulty module
- Power supply issues

**Solutions:**
1. **Verify I2C communication**
   ```cpp
   // Check if DS3231 responds
   if (!rtc.begin()) {
     Serial.println("DS3231 not found!");
   }
   ```

2. **Try manual time setting**
   ```cpp
   // Set specific time
   rtc.adjust(DateTime(2025, 1, 20, 12, 0, 0));
   ```

3. **Check for hardware issues**
   - Try different DS3231 module
   - Verify I2C pull-up resistors (usually built-in)

---

## 🕐 Time/Clock Issues (Updated for DS3231)

### Symptom: Clock shows wrong time after DS3231 upgrade
**Solutions:**
1. **Initial time setting**
   - DS3231 maintains time independently
   - Set time via NTP sync or manual setting
   ```cpp
   synchronizeDS3231WithNTP(); // Preferred method
   ```

2. **Check timezone setting**
   ```cpp
   #define TIME_ZONE_OFFSET 1  // Paris = UTC+1
   ```

3. **Verify NTP synchronization**
   - Check serial monitor for sync messages
   - DS3231 will maintain accurate time between syncs

### Symptom: Time jumps or behaves erratically
**Causes:**
- I2C communication interference
- Power supply instability to DS3231
- Software timing conflicts

**Solutions:**
- Keep I2C wires short and away from LED strips
- Add 100µF capacitor near DS3231 power pins
- Ensure stable 5V power supply
- Check for conflicting timing code

### Symptom: DS3231 temperature reading seems wrong
**Normal Range:** DS3231 temperature sensor accuracy is ±3°C

**Diagnostic:**
1. **Compare with other sensors**
   - DS3231 temperature vs DHT22 readings
   - Should be roughly similar (within 5°C)

2. **Check for heat sources**
   - DS3231 may read higher if near warm components
   - LED strips and Arduino generate heat

**Note:** DS3231 temperature is primarily for internal crystal compensation, not environmental monitoring.

---

## 🔍 Advanced Debugging

### Enable Debug Mode
```cpp
// In config.h:
#define ENABLE_SERIAL_DEBUG true

// In main code:
Serial.begin(115200);
// Watch serial monitor for detailed information
```

### Memory Issues
**Symptom:** Random crashes, strange behavior

**Diagnostic:**
```cpp
// Check free memory:
extern int __heap_start, *__brkval;
int freeMemory = (int) &freeMemory - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
Serial.print("Free memory: ");
Serial.println(freeMemory);
```

**Solutions:**
- Reduce string usage (use F() macro for constants)
- Optimize data structures
- Check for memory leaks in loops

### Serial Debug Output
```cpp
// Add debug prints throughout code:
Serial.print("Function X called, value = ");
Serial.println(someVariable);

// Use different debug levels:
#if ENABLE_SERIAL_DEBUG
  Serial.println("Debug: detailed information");
#endif
```

---

## 📞 When to Seek Help

### Before Asking for Help
✅ **Include this information:**
- Complete error messages from serial monitor
- Photos of your wiring setup
- Your specific hardware configuration
- Steps you've already tried
- Test results from individual component tests

### What Makes a Good Bug Report
1. **Clear problem description**
2. **Expected vs actual behavior**
3. **Reproducible steps**
4. **Hardware/software versions**
5. **Serial monitor output**

### Community Resources
- **GitHub Issues**: For bugs and feature requests
- **Arduino Forums**: For general Arduino help
- **Reddit r/arduino**: Community discussions
- **Discord/Telegram**: Real-time chat support

---

## 🔄 Systematic Testing Approach

When everything fails, return to basics:

1. **Test Arduino alone** → `test_arduino.ino`
2. **Test LCD only** → `test_lcd.ino`  
3. **Test one LED strip** → `test_led.ino`
4. **Test sensors individually** → `test_sensors.ino`
5. **Test buttons** → `test_buttons.ino`
6. **Test RTC** → `test_rtc.ino`
7. **Combine working components gradually**

**Remember:** Most issues are simple wiring problems or configuration errors. Take your time, double-check connections, and test components individually before integrating everything together.