# Web Server Guide

This document describes the web interface and HTTP API of the Smart LED Clock.

## Table of Contents

- [Overview](#overview)
- [Accessing the Web Interface](#accessing-the-web-interface)
- [Web Pages](#web-pages)
- [API Endpoints](#api-endpoints)
- [Configuration API](#configuration-api)
- [Data Formats](#data-formats)
- [Integration Examples](#integration-examples)

## Overview

The Smart LED Clock includes a built-in HTTP web server that provides:

- **Real-time Dashboard:** View current sensor readings and system status
- **Configuration Interface:** Modify settings without re-uploading firmware
- **JSON API:** Programmatic access to data and configuration
- **MQTT Statistics:** Monitor data logging and buffer status
- **Moon Phase Control:** View status and trigger calibration

### Server Details

- **Port:** 80 (standard HTTP)
- **Protocol:** HTTP/1.1
- **Authentication:** None (local network only)
- **Memory:** Optimized with static buffers (no String objects)

### Security Notice

⚠️ **The web server has no authentication.** Only use on trusted local networks. Do not expose to the internet without proper firewall/VPN protection.

## Accessing the Web Interface

### Finding the IP Address

**Method 1: Serial Monitor**
1. Connect Arduino to computer
2. Open Serial Monitor (115200 baud)
3. Look for connection message:
   ```
   WiFi connected!
   IP address: 192.168.1.100
   ```

**Method 2: Router Admin Panel**
1. Access your router's web interface
2. Look for connected devices
3. Find "Arduino" or "smart-led-clock"
4. Note the assigned IP address

**Method 3: Network Scanner**
Use tools like:
- `nmap` (Linux/Mac): `nmap -sn 192.168.1.0/24`
- Angry IP Scanner (Windows)
- Fing (mobile app)

### Connecting

Open a web browser and navigate to:
```
http://<arduino-ip-address>
```

Example:
```
http://192.168.1.100
```

**Supported Browsers:**
- Chrome/Chromium
- Firefox
- Safari
- Edge
- Mobile browsers (iOS Safari, Chrome Mobile)

## Web Pages

### 1. Main Dashboard (`/`)

**URL:** `http://<ip-address>/` or `http://<ip-address>/index`

**Features:**
- Real-time sensor data display
- Current time
- Indoor temperature and humidity
- Outdoor temperature and humidity
- Air quality index with color indicator
- Auto-refresh every 5 seconds (JavaScript)

**Layout:**
```
┌─────────────────────────────────────┐
│     Smart LED Clock Dashboard       │
├─────────────────────────────────────┤
│                                     │
│  Current Time: 14:35:27             │
│                                     │
│  Indoor Environment                 │
│  Temperature: 21.5°C                │
│  Humidity: 45%                      │
│                                     │
│  Outdoor Environment                │
│  Temperature: 15.2°C                │
│  Humidity: 65%                      │
│                                     │
│  Air Quality                        │
│  AQI: 75 (Moderate)                 │
│  [████████░░] 75/100                │
│                                     │
│  [Configuration] [Statistics]       │
└─────────────────────────────────────┘
```

### 2. Configuration Page (`/config`)

**URL:** `http://<ip-address>/config`

**Features:**
- Edit all runtime settings
- Real-time validation
- Save to EEPROM
- Success/error feedback

**Configurable Parameters:**

**Time Settings:**
- Timezone offset (UTC ±12-14)
- NTP sync hour (0-23)
- NTP sync minute (0-59)

**LED Colors (RGB 0-255):**
- Hour hand color
- Minute hand color
- Second hand color

**LED Brightness:**
- Global brightness (0-255)

**Display:**
- LCD backlight timeout (seconds)

**System:**
- Language (French/English)
- Debug mode (on/off)

**Form Example:**
```
┌─────────────────────────────────────┐
│     Clock Configuration             │
├─────────────────────────────────────┤
│                                     │
│  ⏰ Time Settings                    │
│  Timezone offset: [  1  ] (UTC+1)   │
│  NTP sync hour:   [  1  ]           │
│  NTP sync minute: [  1  ]           │
│                                     │
│  💡 LED Colors                       │
│  Hour (RGB):   [  0][  0][255]      │
│  Minute (RGB): [  0][255][  0]      │
│  Second (RGB): [255][  0][  0]      │
│  Brightness:   [100]                │
│                                     │
│  📺 Display                          │
│  LCD timeout: [30] seconds          │
│                                     │
│  [ Save Configuration ]             │
└─────────────────────────────────────┘
```

### 3. Moon Phase Page (`/moon`)

**URL:** `http://<ip-address>/moon`

**Features:**
- Current moon phase name
- Illumination percentage
- Lunar age
- Motor position
- Calibration status
- Manual recalibration button

**Display:**
```
┌─────────────────────────────────────┐
│     Moon Phase Status               │
├─────────────────────────────────────┤
│                                     │
│  Current Phase: Waxing Crescent     │
│  🌒                                  │
│                                     │
│  Illumination: 32.5%                │
│  Lunar Age: 5.2 days                │
│  Motor Position: 512/2048 steps     │
│                                     │
│  Calibrated: ✅ Yes                  │
│  Last Calibration: 2025-01-15       │
│                                     │
│  [ Recalibrate Now ]                │
└─────────────────────────────────────┘
```

### 4. Statistics Page (`/stats`)

**URL:** `http://<ip-address>/stats`

**Features:**
- Data logging statistics
- Buffer usage
- MQTT status
- Connection history

**Display:**
```
┌─────────────────────────────────────┐
│     Data Logging Statistics         │
├─────────────────────────────────────┤
│                                     │
│  MQTT Status: Connected ✅           │
│  Broker: 192.168.1.21:1883          │
│                                     │
│  Buffer Status                      │
│  Current: 0 / 200 points            │
│  Usage: 0%                          │
│  [░░░░░░░░░░░░░░░░░░░░] 0%          │
│                                     │
│  Statistics                         │
│  Total Logged: 1,247 points         │
│  Total Sent: 1,247 points           │
│  Success Rate: 100%                 │
│                                     │
│  Last Activities                    │
│  Last Log: 14:35:23                 │
│  Last Send: 14:35:23                │
└─────────────────────────────────────┘
```

## API Endpoints

All API endpoints return JSON data.

### GET /api/status

**Purpose:** Get current sensor readings

**Method:** GET

**Response:**
```json
{
  "indoor": {
    "temp": 21.5,
    "humidity": 45.0,
    "valid": true
  },
  "outdoor": {
    "temp": 15.2,
    "humidity": 65.0,
    "valid": true
  },
  "airQuality": {
    "aqi": 75,
    "quality": "Moderate"
  },
  "time": "14:35:27"
}
```

**Fields:**
- `indoor.temp` - Indoor temperature (°C)
- `indoor.humidity` - Indoor humidity (%)
- `indoor.valid` - Data validity (true/false)
- `outdoor.temp` - Outdoor temperature (°C)
- `outdoor.humidity` - Outdoor humidity (%)
- `outdoor.valid` - Data validity (true/false)
- `airQuality.aqi` - Air Quality Index (0-500)
- `airQuality.quality` - Quality description (Good, Moderate, Unhealthy, etc.)
- `time` - Current time (HH:MM:SS)

**Usage Example:**
```bash
curl http://192.168.1.100/api/status
```

### GET /api/config

**Purpose:** Get current configuration

**Method:** GET

**Response:**
```json
{
  "timezoneOffset": 1,
  "ntpSyncHour": 1,
  "ntpSyncMinute": 1,
  "led": {
    "hour": {"r": 0, "g": 0, "b": 255},
    "minute": {"r": 0, "g": 255, "b": 0},
    "second": {"r": 255, "g": 0, "b": 0},
    "brightness": 100
  },
  "lcdTimeout": 30000,
  "language": 0,
  "debugMode": 1
}
```

**Fields:**
- `timezoneOffset` - UTC offset in hours (-12 to +14)
- `ntpSyncHour` - Hour for NTP sync (0-23)
- `ntpSyncMinute` - Minute for NTP sync (0-59)
- `led.hour` - Hour hand RGB color (0-255)
- `led.minute` - Minute hand RGB color (0-255)
- `led.second` - Second hand RGB color (0-255)
- `led.brightness` - Global brightness (0-255)
- `lcdTimeout` - Backlight timeout (milliseconds)
- `language` - 0=French, 1=English
- `debugMode` - 0=disabled, 1=enabled

**Usage Example:**
```bash
curl http://192.168.1.100/api/config
```

### POST /api/config

**Purpose:** Update configuration

**Method:** POST

**Content-Type:** `application/x-www-form-urlencoded` or `application/json`

**Request Body (form-encoded):**
```
timezoneOffset=2&ntpSyncHour=2&ledBrightness=150&lcdTimeout=60000
```

**Request Body (JSON):**
```json
{
  "timezoneOffset": 2,
  "ntpSyncHour": 2,
  "ntpSyncMinute": 0,
  "led": {
    "hour": {"r": 0, "g": 0, "b": 200},
    "minute": {"r": 0, "g": 200, "b": 0},
    "second": {"r": 200, "g": 0, "b": 0},
    "brightness": 150
  },
  "lcdTimeout": 60000
}
```

**Response (Success):**
```json
{
  "success": true,
  "message": "Configuration saved"
}
```

**Response (Error):**
```json
{
  "success": false,
  "message": "Save failed"
}
```

**Usage Example:**
```bash
# Update timezone and brightness
curl -X POST http://192.168.1.100/api/config \
  -d "timezoneOffset=2&ledBrightness=150"
```

**Notes:**
- Changes are applied immediately
- Configuration is saved to EEPROM
- Some settings (language, debug mode) require restart

### GET /api/logstats

**Purpose:** Get data logging statistics

**Method:** GET

**Response:**
```json
{
  "bufferCount": 0,
  "bufferMax": 200,
  "bufferUsage": 0,
  "totalLogged": 1247,
  "totalSent": 1247,
  "mqttConnected": true,
  "lastLogTime": 1234567890,
  "lastSendTime": 1234567890
}
```

**Fields:**
- `bufferCount` - Current points in buffer
- `bufferMax` - Maximum buffer capacity
- `bufferUsage` - Usage percentage (0-100)
- `totalLogged` - Total points logged since boot
- `totalSent` - Total points sent via MQTT
- `mqttConnected` - MQTT connection status
- `lastLogTime` - Last log timestamp (milliseconds)
- `lastSendTime` - Last send timestamp (milliseconds)

**Usage Example:**
```bash
curl http://192.168.1.100/api/logstats
```

### GET /api/history

**Purpose:** Get buffered data points

**Method:** GET

**Query Parameters:**
- `count` - Number of points to retrieve (default: 20, max: 20)

**Response:**
```json
{
  "count": 3,
  "points": [
    {
      "timestamp": 1705329600,
      "indoor": {"temp": 21.5, "humidity": 45.0},
      "outdoor": {"temp": 15.2, "humidity": 65.0},
      "aqi": 75
    },
    {
      "timestamp": 1705329300,
      "indoor": {"temp": 21.4, "humidity": 45.5},
      "outdoor": {"temp": 15.1, "humidity": 65.2},
      "aqi": 73
    },
    {
      "timestamp": 1705329000,
      "indoor": {"temp": 21.3, "humidity": 46.0},
      "outdoor": {"temp": 15.0, "humidity": 65.5},
      "aqi": 72
    }
  ]
}
```

**Usage Example:**
```bash
# Get last 10 buffered points
curl http://192.168.1.100/api/history?count=10
```

**Notes:**
- Returns data from RAM buffer (not real-time)
- Useful when WiFi was offline
- Maximum 20 points for memory safety

### GET /api/moon

**Purpose:** Get moon phase information

**Method:** GET

**Query Parameters:**
- `action` - Optional action: "status" (default) or "recalibrate"

**Response (status):**
```json
{
  "phase": 1,
  "phaseName": "Waxing Crescent",
  "exactPhase": 1.45,
  "illumination": 32.5,
  "lunarAge": 5.2,
  "currentSteps": 512,
  "calibrated": true,
  "lastCalibration": 1705267200
}
```

**Response (recalibrate):**
```json
{
  "success": true,
  "message": "Calibration started",
  "previousPosition": 512,
  "newPosition": 0,
  "peakValue": 850,
  "stepsTaken": 2048
}
```

**Fields:**
- `phase` - Phase number (0-7)
- `phaseName` - Phase description
- `exactPhase` - Continuous phase (0.0-8.0)
- `illumination` - Illumination percentage (0-100)
- `lunarAge` - Age in days (0-29.53)
- `currentSteps` - Motor position (0-2047)
- `calibrated` - Calibration status
- `lastCalibration` - Last calibration timestamp

**Usage Example:**
```bash
# Get moon status
curl http://192.168.1.100/api/moon?action=status

# Trigger recalibration
curl http://192.168.1.100/api/moon?action=recalibrate
```

**Notes:**
- Recalibration takes ~2 minutes (full rotation)
- Status available immediately
- Moon module must be enabled in config

## Configuration API

### Updating Configuration via API

The `/api/config` endpoint accepts both form-encoded and JSON data.

#### Example 1: Update Timezone

```bash
curl -X POST http://192.168.1.100/api/config \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "timezoneOffset=2"
```

#### Example 2: Update LED Colors

```bash
curl -X POST http://192.168.1.100/api/config \
  -H "Content-Type: application/json" \
  -d '{
    "led": {
      "hour": {"r": 100, "g": 0, "b": 200},
      "brightness": 150
    }
  }'
```

#### Example 3: Update Multiple Settings

```bash
curl -X POST http://192.168.1.100/api/config \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "timezoneOffset=1&ledBrightness=120&lcdTimeout=45000"
```

### Configuration Validation

The API validates all inputs:

- **Timezone:** -12 to +14
- **NTP hour:** 0 to 23
- **NTP minute:** 0 to 59
- **RGB values:** 0 to 255
- **Brightness:** 0 to 255
- **LCD timeout:** 0 to 4294967295 (uint32_t max)
- **Language:** 0 (French) or 1 (English)
- **Debug mode:** 0 (off) or 1 (on)

Invalid values are rejected with error response.

## Data Formats

### Timestamp Format

All timestamps are Unix epoch (seconds since 1970-01-01):

```json
"timestamp": 1705329600  // 2025-01-15 14:00:00 UTC
```

Convert in JavaScript:
```javascript
const date = new Date(timestamp * 1000);
console.log(date.toLocaleString());
```

### Time Format

Current time is formatted as HH:MM:SS string:

```json
"time": "14:35:27"
```

### Temperature and Humidity

- Temperature: Float, degrees Celsius
- Humidity: Float, percentage (0-100)

```json
"temp": 21.5,
"humidity": 45.0
```

### Air Quality Index (AQI)

Integer value (0-500):

| Range | Quality | Color |
|-------|---------|-------|
| 0-50 | Good | Green |
| 51-100 | Moderate | Yellow |
| 101-150 | Unhealthy (sensitive) | Orange |
| 151-200 | Unhealthy | Red |
| 201-300 | Very Unhealthy | Purple |
| 301+ | Hazardous | Maroon |

## Integration Examples

### Python Integration

```python
import requests
import json

# Base URL
BASE_URL = "http://192.168.1.100"

# Get current sensor data
response = requests.get(f"{BASE_URL}/api/status")
data = response.json()

print(f"Indoor: {data['indoor']['temp']}°C, {data['indoor']['humidity']}%")
print(f"Outdoor: {data['outdoor']['temp']}°C, {data['outdoor']['humidity']}%")
print(f"Air Quality: {data['airQuality']['aqi']} ({data['airQuality']['quality']})")

# Update LED brightness
config_data = {"led": {"brightness": 150}}
response = requests.post(
    f"{BASE_URL}/api/config",
    json=config_data
)
print(response.json())
```

### JavaScript/Node.js Integration

```javascript
const axios = require('axios');

const BASE_URL = 'http://192.168.1.100';

// Get sensor data
async function getSensorData() {
  const response = await axios.get(`${BASE_URL}/api/status`);
  return response.data;
}

// Update configuration
async function updateConfig(config) {
  const response = await axios.post(`${BASE_URL}/api/config`, config);
  return response.data;
}

// Example usage
getSensorData().then(data => {
  console.log('Indoor:', data.indoor);
  console.log('Outdoor:', data.outdoor);
  console.log('AQI:', data.airQuality.aqi);
});

updateConfig({led: {brightness: 150}}).then(result => {
  console.log('Config updated:', result.message);
});
```

### Home Assistant Integration

Add to `configuration.yaml`:

```yaml
sensor:
  - platform: rest
    name: "Clock Indoor Temperature"
    resource: "http://192.168.1.100/api/status"
    value_template: "{{ value_json.indoor.temp }}"
    unit_of_measurement: "°C"
    
  - platform: rest
    name: "Clock Outdoor Temperature"
    resource: "http://192.168.1.100/api/status"
    value_template: "{{ value_json.outdoor.temp }}"
    unit_of_measurement: "°C"
    
  - platform: rest
    name: "Clock Air Quality"
    resource: "http://192.168.1.100/api/status"
    value_template: "{{ value_json.airQuality.aqi }}"
    unit_of_measurement: "AQI"
```

### Shell Script Monitoring

```bash
#!/bin/bash
# monitor-clock.sh - Monitor Smart LED Clock

CLOCK_IP="192.168.1.100"

while true; do
  DATA=$(curl -s "http://$CLOCK_IP/api/status")
  
  INDOOR_TEMP=$(echo $DATA | jq -r '.indoor.temp')
  OUTDOOR_TEMP=$(echo $DATA | jq -r '.outdoor.temp')
  AQI=$(echo $DATA | jq -r '.airQuality.aqi')
  
  echo "$(date) - Indoor: ${INDOOR_TEMP}°C, Outdoor: ${OUTDOOR_TEMP}°C, AQI: $AQI"
  
  sleep 60  # Check every minute
done
```

## Troubleshooting

### Cannot Access Web Interface

**Problem:** Browser cannot reach `http://<ip-address>`

**Solutions:**
1. Verify Arduino is powered on
2. Check WiFi connection (Serial Monitor)
3. Ping the IP address: `ping 192.168.1.100`
4. Ensure computer and Arduino are on same network
5. Check firewall settings
6. Try different browser

### Page Loads but Shows Old Data

**Problem:** Dashboard displays outdated information

**Solutions:**
1. Hard refresh browser (Ctrl+F5 or Cmd+Shift+R)
2. Clear browser cache
3. Check JavaScript console for errors
4. Verify sensors are working (Serial Monitor)
5. Restart Arduino

### Configuration Changes Not Saving

**Problem:** Settings revert after reboot

**Solutions:**
1. Wait for "Configuration saved" message
2. Check EEPROM write success (Serial Monitor)
3. Verify values are within valid ranges
4. Try smaller configuration changes
5. Check available EEPROM space

### API Returns Empty Response

**Problem:** `/api/*` endpoints return nothing

**Solutions:**
1. Check URL is exactly correct (case-sensitive)
2. Verify Content-Type header for POST
3. Check Arduino Serial Monitor for errors
4. Ensure sufficient memory (restart if needed)
5. Test with curl or Postman instead of browser

### Slow Response Times

**Problem:** Web pages load slowly

**Solutions:**
1. Reduce LED brightness (less power, more stable)
2. Increase power supply capacity
3. Reduce concurrent connections
4. Disable debug mode for production
5. Check network signal strength

---

**Next Steps:**
- For testing the web interface, see [Testing Guide](../testing/README.md)
- For configuration details, see [Configuration Guide](CONFIGURATION.md)
- For software architecture, see [Software Architecture](SOFTWARE.md)
