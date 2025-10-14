# Software Tests - Smart LED Clock 💻

> Software validation, integration testing, and system verification procedures

This document covers software-level testing including WiFi connectivity, NTP synchronization, web interface, and full system integration.

---

## 📋 Table of Contents

- [Prerequisites](#prerequisites)
- [Software Testing Overview](#software-testing-overview)
- [WiFi and Network Tests](#wifi-and-network-tests)
- [Web Interface Tests](#web-interface-tests)
- [System Integration Tests](#system-integration-tests)
- [Performance Tests](#performance-tests)
- [Long-term Stability Tests](#long-term-stability-tests)

---

## Prerequisites

### Before Software Testing

Ensure all hardware tests have passed:
- ✅ Arduino board test complete
- ✅ All LED strips working
- ✅ LCD display functional
- ✅ Buttons responsive
- ✅ RTC module keeping time
- ✅ Sensors providing readings

### Required Software

```
Arduino IDE: 1.8.19+ or 2.x
Libraries installed:
  - Adafruit NeoPixel
  - LiquidCrystal I2C
  - RTClib
  - DHT sensor library
  - OneButton
  - WiFiS3 (built-in)

WiFi Network:
  - 2.4 GHz network (Arduino R4 doesn't support 5GHz)
  - WPA2 security supported
  - DHCP enabled (or static IP configured)
  - Internet access for NTP sync
```

### Network Requirements

**Router/Access Point:**
- 2.4 GHz WiFi (802.11 b/g/n)
- WPA/WPA2 security
- DHCP server enabled
- Port 123 open (NTP)
- Port 80 accessible (web interface)

**Firewall:**
- Allow outgoing UDP port 123 (NTP)
- Allow incoming TCP port 80 (web interface)

---

## Software Testing Overview

### Testing Phases

```
Phase 1: WiFi Connectivity
  ├─ SSID detection
  ├─ Authentication
  ├─ IP assignment
  └─ Connection stability

Phase 2: Network Services
  ├─ NTP synchronization
  ├─ Time zone handling
  └─ Auto-reconnection

Phase 3: Web Interface
  ├─ Web server operation
  ├─ Dashboard functionality
  ├─ Configuration page
  └─ API endpoints

Phase 4: System Integration
  ├─ All components working together
  ├─ Feature interactions
  ├─ Mode switching
  └─ Error handling

Phase 5: Performance
  ├─ Memory usage
  ├─ CPU utilization
  ├─ Response times
  └─ Power consumption

Phase 6: Stability
  ├─ 24-hour operation test
  ├─ Power cycle recovery
  ├─ WiFi reconnection
  └─ Error recovery
```

---

## WiFi and Network Tests

### Test 1: WiFi Configuration

**Purpose:** Verify WiFi credentials are correctly configured

**Procedure:**

1. **Create secrets files** (if not already done)
   ```cpp
   // secrets.h
   #ifndef SECRETS_H
   #define SECRETS_H
   
   extern const char* ssid;
   extern const char* pass;
   
   #endif
   ```
   
   ```cpp
   // secrets.cpp
   #include "secrets.h"
   
   const char* ssid = "YourNetworkName";
   const char* pass = "YourNetworkPassword";
   ```

2. **Upload main firmware**
   ```
   File → Open → smart-led-clock.ino
   Sketch → Upload
   Tools → Serial Monitor (115200 baud)
   ```

3. **Observe boot sequence**
   Look for WiFi initialization messages:
   ```
   === Smart LED Clock ===
   Initializing WiFi...
   Connecting to: YourNetworkName
   ```

**Success Criteria:**
- ✅ SSID name appears correctly
- ✅ No "incorrect password" errors
- ✅ Connection attempt proceeds

**Common Issues:**
| Problem | Solution |
|---------|----------|
| SSID not found | Check network name, verify 2.4GHz |
| Authentication failed | Verify password, check WPA2 support |
| secrets.h not found | Ensure secrets.cpp is in project folder |

---

### Test 2: IP Address Assignment

**Purpose:** Verify Arduino receives IP address via DHCP

**Procedure:**

1. **Monitor serial output during boot**
   ```
   Connecting to WiFi....
   WiFi connected!
   IP address: 192.168.1.150
   MAC address: XX:XX:XX:XX:XX:XX
   ```

2. **Verify IP address**
   - Should be in your network's subnet
   - Typical: 192.168.1.x or 192.168.0.x
   - Not 0.0.0.0 (connection failed)

3. **Test connectivity from computer**
   ```bash
   # Windows
   ping 192.168.1.150
   
   # macOS/Linux
   ping 192.168.1.150
   ```

**Success Criteria:**
- ✅ Valid IP address assigned
- ✅ IP in correct subnet
- ✅ Arduino responds to ping
- ✅ Connection status LED (if implemented)

**Troubleshooting:**
| Problem | Solution |
|---------|----------|
| IP shows 0.0.0.0 | DHCP failed, check router settings |
| Can't ping Arduino | Check firewall, verify same network |
| IP changes on reboot | Configure DHCP reservation in router |

---

### Test 3: NTP Synchronization

**Purpose:** Verify clock synchronizes time with NTP server

**Procedure:**

1. **Monitor first NTP sync** (happens at boot)
   ```
   Initializing RTC...
   Attempting NTP sync...
   NTP sync successful!
   Time set to: 2025/01/15 14:32:45
   Next sync at: 01:01
   ```

2. **Verify time accuracy**
   - Compare serial output time to actual time
   - Should be accurate within 1-2 seconds
   - Check timezone offset applied correctly

3. **Check automatic sync**
   - Default: Daily at 01:01 AM
   - Monitor serial for sync message next day
   - Or change sync time in config.h for testing

4. **Test manual sync** (via web interface)
   - Access: http://[IP-address]
   - Navigate to Settings
   - Click "Sync Time Now"
   - Verify success message

**NTP Servers Used:**
```
Primary: pool.ntp.org
Fallback: time.nist.gov
Timeout: 5 seconds
Retry: 3 attempts
```

**Success Criteria:**
- ✅ NTP sync succeeds at boot
- ✅ Time matches actual time (within 2 seconds)
- ✅ Timezone applied correctly
- ✅ Daily automatic sync works
- ✅ Manual sync via web interface works

**Troubleshooting:**
| Problem | Solution |
|---------|----------|
| NTP timeout | Check internet connectivity, try different NTP server |
| Wrong timezone | Verify TIME_ZONE_OFFSET in config.h |
| Time drifts | DS3231 should maintain accuracy, check battery |
| Sync fails repeatedly | Firewall blocking UDP port 123 |

---

### Test 4: Connection Stability

**Purpose:** Verify WiFi connection remains stable over time

**Procedure:**

1. **Monitor for 1 hour**
   - Leave Arduino running
   - Check serial output every 10 minutes
   - Note any disconnections

2. **Check connection status**
   ```
   WiFi Status: Connected
   Signal Strength: -65 dBm (good)
   Uptime: 3600 seconds
   Reconnections: 0
   ```

3. **Test reconnection** (optional)
   - Temporarily disable WiFi on router
   - Wait 30 seconds
   - Re-enable WiFi
   - Arduino should reconnect automatically

**Signal Strength Guide:**
- -30 to -50 dBm: Excellent
- -50 to -60 dBm: Very good
- -60 to -70 dBm: Good
- -70 to -80 dBm: Fair
- -80 to -90 dBm: Poor
- Below -90 dBm: Unreliable

**Success Criteria:**
- ✅ Connection remains stable for 1+ hours
- ✅ Signal strength adequate (> -80 dBm)
- ✅ Auto-reconnection works after network disruption
- ✅ No memory leaks or crashes

---

## Web Interface Tests

### Test 5: Web Server Accessibility

**Purpose:** Verify web server responds to HTTP requests

**Procedure:**

1. **Find Arduino IP address**
   - Check serial monitor at boot
   - Or check router's DHCP client list

2. **Access from web browser**
   ```
   http://192.168.1.150
   ```

3. **Verify homepage loads**
   - Should display dashboard
   - Shows current time
   - Shows sensor readings
   - Navigation buttons visible

**Success Criteria:**
- ✅ Web page loads within 2 seconds
- ✅ HTML renders correctly
- ✅ No 404 or 500 errors
- ✅ CSS styling applied
- ✅ Page is responsive

**Common Issues:**
| Problem | Solution |
|---------|----------|
| Connection refused | Verify IP address, check web server started |
| Page doesn't load | Check browser, try different browser |
| Partial content | Memory issue, restart Arduino |
| Styles missing | CSS not serving correctly, check webpage.h |

---

### Test 6: Dashboard Functionality

**Purpose:** Verify dashboard displays real-time data correctly

**Test Areas:**

**6.1 Time Display**
- Current time shows in HH:MM:SS format
- Time updates every second (via JavaScript)
- Date shows correctly (DD/MM/YYYY)
- Day of week displays in correct language

**6.2 Temperature Readings**
- Indoor temperature displays
- Outdoor temperature displays
- Values update every 5 seconds
- Units show °C or °F based on settings

**6.3 Humidity Readings**
- Indoor humidity percentage
- Outdoor humidity percentage
- Values reasonable (30-80% typical)

**6.4 Air Quality Display**
- AQI value shows
- Color indicator changes based on AQI:
  - Green: 0-50 (Good)
  - Yellow: 51-100 (Moderate)
  - Orange: 101-150 (Unhealthy for sensitive)
  - Red: 151-200 (Unhealthy)
  - Purple: 201-300 (Very unhealthy)
  - Maroon: 301+ (Hazardous)

**6.5 Auto-Refresh**
- Data updates automatically every 5 seconds
- No page reload required
- AJAX requests succeed
- No console errors in browser

**Success Criteria:**
- ✅ All sensor values display
- ✅ Values update in real-time
- ✅ No "N/A" or error values (except during sensor errors)
- ✅ Auto-refresh works
- ✅ Time stays synchronized

---

### Test 7: Configuration Page

**Purpose:** Verify settings can be changed via web interface

**Test Areas:**

**7.1 Display Settings**
- LED Brightness (0-255 slider)
  - Move slider, click Save
  - Verify LEDs change brightness immediately
- LCD Timeout (seconds)
  - Set timeout value
  - Verify LCD turns off after timeout
- LCD Backlight toggle
  - Turn off, verify LCD backlight off
  - Turn on, verify LCD backlight on

**7.2 Time Settings**
- Timezone offset (-12 to +14)
  - Change timezone
  - Click Save and Restart
  - Verify time updates after restart
- NTP Sync Time
  - Set hour and minute
  - Save and restart
  - Verify sync happens at set time

**7.3 LED Colors**
- Hour color (RGB sliders)
- Minute color (RGB sliders)
- Second color (RGB sliders)
- Changes apply immediately
- Click Save to persist

**7.4 Language**
- Switch between French/English
- Save and restart
- Verify LCD displays in selected language

**Success Criteria:**
- ✅ All settings save correctly
- ✅ Changes persist after restart
- ✅ Immediate-apply settings work without restart
- ✅ Restart-required settings work after restart
- ✅ No settings corruption

---

### Test 8: API Endpoints

**Purpose:** Verify REST API functions correctly

**Test Using curl (or browser):**

**8.1 Status Endpoint**
```bash
curl http://192.168.1.150/api/status
```

Expected response:
```json
{
  "time": "14:32:45",
  "date": "15/01/2025",
  "temperature": {
    "indoor": 22.5,
    "outdoor": 18.3
  },
  "humidity": {
    "indoor": 65,
    "outdoor": 56
  },
  "airQuality": {
    "aqi": 42,
    "quality": "Good"
  }
}
```

**8.2 Config Endpoint (GET)**
```bash
curl http://192.168.1.150/api/config
```

Expected response:
```json
{
  "ledBrightness": 100,
  "lcdTimeout": 30000,
  "timezone": 2,
  "language": "EN",
  "debugMode": false
}
```

**8.3 Config Endpoint (POST)**
```bash
curl -X POST http://192.168.1.150/api/config \
  -H "Content-Type: application/json" \
  -d '{"ledBrightness": 50}'
```

Expected response:
```json
{
  "success": true,
  "message": "Configuration updated",
  "restart_required": false
}
```

**Success Criteria:**
- ✅ All endpoints return valid JSON
- ✅ Status data matches actual sensor values
- ✅ Config changes apply correctly
- ✅ HTTP status codes correct (200, 400, 500)
- ✅ Error handling works (invalid JSON, out of range values)

---

## System Integration Tests

### Test 9: Component Interaction

**Purpose:** Verify all components work together harmoniously

**Test Scenarios:**

**9.1 Button → LCD Mode Change**
- Press button
- Verify LCD changes display mode
- Check serial output for mode change log
- Verify backlight turns on

**9.2 Button → LCD Backlight**
- Wait for LCD timeout (LCD off)
- Press button
- Verify LCD turns on immediately

**9.3 Hourly Animation**
- Wait for top of hour (e.g., 14:00:00)
- Verify animation triggers
- Verify LCD shows "Animation horaire"
- Verify LEDs show rainbow pattern
- After animation, clock resumes normal display

**9.4 Air Quality → LED Bar**
- Change air quality (breathe on MQ135)
- Verify LED bar color changes:
  - Green → Good air
  - Yellow → Moderate
  - Red → Poor
- Verify brightness adjusts

**9.5 Web Config → Hardware**
- Change LED brightness via web
- Verify LEDs change immediately
- Change LCD timeout
- Verify LCD respects new timeout

**Success Criteria:**
- ✅ Button presses always register
- ✅ LCD responds to button correctly
- ✅ Hourly animation triggers on time
- ✅ Air quality bar reflects sensor
- ✅ Web changes affect hardware
- ✅ No component interference

---

### Test 10: Error Handling

**Purpose:** Verify system handles errors gracefully

**Test Scenarios:**

**10.1 Sensor Failures**
- Disconnect DHT22 indoor sensor
- Verify LCD shows "ERREUR" for indoor temp
- Verify outdoor sensor still works
- Reconnect sensor
- Verify recovery without restart

**10.2 WiFi Loss**
- Disable WiFi router
- Verify clock continues working (RTC fallback)
- Verify web interface unavailable
- Re-enable WiFi
- Verify auto-reconnection
- Verify web interface restores

**10.3 NTP Failure**
- Block UDP port 123 (firewall)
- Trigger NTP sync
- Verify fallback to RTC time
- Verify error logged
- Unblock port
- Verify next sync succeeds

**10.4 Invalid Web Inputs**
- Try setting brightness to 300 (max is 255)
- Verify rejection with error message
- Try timezone offset of 20 (max is 14)
- Verify validation works

**Success Criteria:**
- ✅ System continues operating despite individual failures
- ✅ Errors logged to serial
- ✅ Automatic recovery when possible
- ✅ User-friendly error messages
- ✅ No crashes or reboots

---

## Performance Tests

### Test 11: Memory Usage

**Purpose:** Monitor RAM and flash memory utilization

**Procedure:**

1. **Check compilation output**
   ```
   Sketch uses 85432 bytes (XX%) of program storage space.
   Global variables use 12456 bytes (XX%) of dynamic memory.
   ```

2. **Monitor runtime memory** (if implemented)
   Serial output may show:
   ```
   Free RAM: 18524 bytes
   ```

**Memory Guidelines:**
- Flash (Program): < 90% (crashes if full)
- RAM (Dynamic): Leave 5KB+ free for stack/heap
- Fragments: Minimize dynamic allocation

**Success Criteria:**
- ✅ Flash usage < 90%
- ✅ RAM usage < 80%
- ✅ No "out of memory" errors
- ✅ No memory leaks over 24 hours

---

### Test 12: Response Times

**Purpose:** Measure system responsiveness

**Measurements:**

**Web Interface:**
- Dashboard load time: < 2 seconds
- AJAX update time: < 500ms
- Config save time: < 1 second

**Button Response:**
- Button press to LED update: < 100ms
- Button press to LCD update: < 200ms
- Long press detection: 1000ms (by design)

**Sensor Updates:**
- DHT22 reading cycle: 2 seconds (sensor limitation)
- MQ135 reading: Instant (analog)
- Display refresh: < 500ms

**Success Criteria:**
- ✅ Web page loads quickly
- ✅ Button response feels instant
- ✅ Display updates smoothly
- ✅ No noticeable lag

---

## Long-term Stability Tests

### Test 13: 24-Hour Operation

**Purpose:** Verify stable operation over extended period

**Procedure:**

1. **Start test**
   - Note start time
   - Record initial sensor values
   - Check free memory

2. **Monitor periodically** (every 4-6 hours)
   - Check serial output for errors
   - Verify web interface accessible
   - Check sensor readings reasonable
   - Note free memory (check for leaks)

3. **After 24 hours**
   - Verify clock still accurate
   - Check total uptime
   - Review error log
   - Compare memory usage

**Log Template:**
```
Hour 0:  Start - RAM: 18524 bytes
Hour 4:  Check - RAM: 18510 bytes, Errors: 0
Hour 8:  Check - RAM: 18502 bytes, Errors: 0
Hour 12: Check - RAM: 18498 bytes, Errors: 0
Hour 16: Check - RAM: 18490 bytes, Errors: 0
Hour 20: Check - RAM: 18485 bytes, Errors: 0
Hour 24: End   - RAM: 18480 bytes, Errors: 0

Time drift: +0.5 seconds
WiFi disconnections: 0
Sensor errors: 0
Web requests served: 143
```

**Success Criteria:**
- ✅ No crashes or reboots
- ✅ Time drift < 5 seconds
- ✅ Memory stable (no leaks)
- ✅ WiFi stays connected
- ✅ All features operational
- ✅ Error rate < 1%

---

### Test 14: Power Cycle Recovery

**Purpose:** Verify system recovers from power loss

**Procedure:**

1. **Record state**
   - Note current settings
   - Check RTC time
   - Note sensor calibrations (if any)

2. **Power cycle**
   - Disconnect power (USB + external)
   - Wait 10 seconds
   - Reconnect power

3. **Verify recovery**
   - System boots successfully
   - WiFi reconnects
   - RTC time maintained (battery backup)
   - Settings restored from EEPROM
   - All features operational

4. **Repeat 5 times**
   - Verify consistent recovery
   - No degradation after multiple cycles

**Success Criteria:**
- ✅ Boots successfully every time
- ✅ Time maintained (thanks to DS3231 battery)
- ✅ Settings restored from EEPROM
- ✅ WiFi reconnects automatically
- ✅ All features resume operation
- ✅ No configuration loss

---

## Test Results Documentation

### Test Report Template

```markdown
# Software Test Results - Smart LED Clock

**Date:** January 15, 2025
**Tester:** [Your Name]
**Firmware Version:** 1.1.0
**Arduino IDE:** 2.2.1

## Environment
- WiFi Network: [Network Name]
- Router: [Router Model]
- Signal Strength: -65 dBm
- Internet: Available

## Test Results

### WiFi and Network
- [✅] WiFi Configuration
- [✅] IP Assignment
- [✅] NTP Synchronization
- [✅] Connection Stability

### Web Interface
- [✅] Web Server Access
- [✅] Dashboard Functionality
- [✅] Configuration Page
- [✅] API Endpoints

### System Integration
- [✅] Component Interaction
- [✅] Error Handling

### Performance
- [✅] Memory Usage: 78% Flash, 42% RAM
- [✅] Response Times: All under target
- [✅] Web Performance: Dashboard loads in 1.2s

### Stability
- [✅] 24-Hour Test: No errors, stable operation
- [✅] Power Cycle: Recovered perfectly 5/5 times

## Issues Found
1. Minor CSS alignment issue on mobile - Non-critical
2. NTP timeout on first attempt - Recovered on retry

## Recommendations
- All tests passed
- System ready for production use
- Consider adding connection retry counter to dashboard

## Overall Assessment
✅ PASS - All critical tests passed successfully
System is stable and ready for deployment.
```

---

## Next Steps

After all software tests pass:

1. **Final System Validation**
   - Run all hardware and software tests once more
   - Document final configuration
   - Take photos of setup

2. **Deploy to Final Location**
   - Mount components securely
   - Arrange cables neatly
   - Position sensors appropriately

3. **Monitor Initial Operation**
   - Watch first 48 hours closely
   - Fine-tune settings as needed
   - Document any issues

4. **Create User Documentation**
   - Operation instructions
   - Troubleshooting guide
   - Maintenance schedule

---

**Author:** F. Baillon  
**Version:** 1.0.0  
**Last Updated:** January 2025