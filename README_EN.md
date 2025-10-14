# Smart LED Clock - English Documentation 🇬🇧

> Smart LED clock with environmental sensors and NTP synchronization

[![Version](https://img.shields.io/badge/version-1.1.0-blue.svg)](https://github.com/fbaillon19/smart-led-clock)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](./LICENSE)
[![Arduino](https://img.shields.io/badge/Arduino-Uno%20R4%20WiFi-00979D.svg)](https://www.arduino.cc/)

[🇫🇷 Version Française](./README_FR.md) | [📚 Complete Documentation](./docs/)

---

## 🎯 Overview

Smart LED Clock is a connected clock that combines an elegant analog display on LED rings with environmental sensors, a complete web interface, and real-time configuration.

### Key Features

- ⏰ **LED Analog Display**: Two rings (12 LED hours, 60 LED minutes/seconds)
- 📺 **20x4 LCD Display**: 3 interchangeable display modes
- 🌡️ **Environmental Sensors**: Temperature and humidity (indoor/outdoor)
- 💨 **Air Quality**: MQ135 sensor with colored LED bar
- 🌐 **WiFi Connectivity**: Automatic NTP synchronization
- 🌐 **Complete Web Interface**: Dashboard and configuration via browser (v1.1.0+)
- 🔘 **Physical Control**: Single button for LCD display
- 🎨 **Animations**: Light effect every hour on the hour
- 💾 **EEPROM Backup**: Persistent configuration

### Display Modes Preview

**Mode 1 - Temperature & Humidity** (default)
```
  SAT 03 OCT 2025
     14:32:45
INT:22.5°C   65% AQI
EXT:18.3°C   56%  42
```

**Mode 2 - Feels Like Temperature**
```
   FEELS LIKE TEMP.
 OUTDOOR: 18.3°C
FEELS LIKE: 17.1°C
 DEW POINT: 12.4°C
```

**Mode 3 - Humidex Index**
```
   HUMIDEX INDEX
       123
 SIGNIFICANT
    DISCOMFORT
OUTDOOR ONLY
```

## 🌐 Web Interface (v1.1.0+)

The clock features a **complete web interface** accessible from your browser.

### Access

1. Connect to the same WiFi network as the clock
2. Find the Arduino's IP address (displayed at startup in the serial monitor)
3. Open your browser: `http://192.168.x.x`

### Available Pages

#### 📊 Dashboard (Home Page)
- Real-time sensor display
- Automatic refresh every 5 seconds
- Indoor and outdoor temperatures
- Indoor and outdoor humidity
- Air quality (AQI)
- Current time

#### ⚙️ Configuration
Accessible via the "Configuration" button on the dashboard.

**Configurable Parameters:**
- **Time Zone**: UTC offset (-12 to +14)
- **NTP Synchronization**: Daily sync hour and minute
- **LED Colors**: Customizable RGB for hours/minutes/seconds
- **Brightness**: 0-255 (immediate change)
- **LCD Timeout**: 5-300 seconds before turning off
- **Language**: French or English
- **Debug Mode**: Enable/disable serial logs

**✨ Real-time Changes:**
The following parameters are applied immediately without restart:
- LED brightness
- LED colors
- LCD backlight timeout

**⚠️ Requires Restart:**
- Time zone and NTP synchronization time
- Display language
- Debug mode

### REST API

For advanced integrations, the clock exposes a REST API:
```bash
# Get sensor status
curl http://192.168.x.x/api/status

# Get configuration
curl http://192.168.x.x/api/config

# Modify configuration
curl -X POST http://192.168.x.x/api/config \
  -H "Content-Type: application/json" \
  -d '{"ledBrightness": 50, "lcdTimeout": 60000}'
```

---

## ⚡ Quick Start

### 1. Required Hardware

**Main Components:**
- Arduino Uno R4 WiFi
- I2C LCD Display 20x4
- 2× DHT22 (temperature/humidity)
- MQ135 Sensor (air quality)
- 3× NeoPixel LED Rings/Bars (12, 60, 10 LEDs)
- DS3231 RTC Module
- Push Button

📋 **[Complete List and Schematics → docs/HARDWARE.md](./docs/HARDWARE.md)**

### 2. Installation

```bash
# 1. Clone the repository
git clone https://github.com/fbaillon19/smart-led-clock.git
cd smart-led-clock

# 2. Create WiFi configuration file
cp firmware/smart-led-clock/secrets.template.h firmware/smart-led-clock/secrets.h
touch firmware/smart-led-clock/secrets.cpp
# Edit secrets.cpp with your WiFi credentials

# 3. Open in Arduino IDE
# File: firmware/smart-led-clock/smart-led-clock.ino

# 4. Install required libraries
# See docs/INSTALL.md for complete list

# 5. Compile and upload
```

🔧 **[Detailed Installation Guide → docs/INSTALL.md](./docs/INSTALL.md)**

### 3. WiFi Configuration

Edit `firmware/smart-led-clock/secrets.h`:
```cpp
const char* ssid = "YourSSID";
const char* pass = "YourPassword";
```

### 4. First Start

1. Connect the Arduino to PC via USB
2. Open serial monitor (115200 baud)
3. Observe initialization sequence
4. The clock automatically synchronizes via NTP

---

## 🎮 Usage

### Button Control

| Action | Result |
|--------|--------|
| **Short Click** (LCD off) | Turn on backlight |
| **Short Click** (LCD on) | Change display mode |
| **Long Press** (>2s) | Return to default mode |

### Automatic Features

- 💡 **LCD Backlight**: Automatic turn-off after 30 seconds
- 🌐 **NTP Synchronization**: Daily at 1:01 AM
- 🎨 **Animation**: Every hour on the hour (14:00, 15:00...)
- 📊 **Sensor Update**: Every 2 seconds

---

## 🏗️ Software Architecture

The project uses a **modular architecture**:

```
firmware/smart-led-clock/
├── smart-led-clock.ino      # Main program
├── config.h                 # Centralized configuration
├── strings.h                # Internationalization
├── leds.h / leds.cpp       # LED rings + animations
├── rtc.h / rtc.cpp         # RTC + WiFi + NTP
├── sensors.h / sensors.cpp  # Sensors (DHT22, MQ135)
├── display.h / display.cpp  # LCD and display modes
├── button.h / button.cpp   # Button management
├── webserver.h / webserver.cpp /webpage.h   # Web server
├── storage.h / storage.cpp   # EEPROM storage
└── secrets.h / secrets.cpp  # Sensitive data confidentiality
```

### Architecture Benefits

✅ **Modular**: Each feature in its own module  
✅ **Maintainable**: Organized and commented code  
✅ **Scalable**: Easy to add new features  
✅ **Testable**: Independent modules  

---

## ⚙️ Configuration

### Via Web Interface (Recommended)

Access `http://[clock-IP-address]` and use the configuration page to modify all parameters in real-time.

### Advanced Customization

Edit `config.h` to modify:

**Time Zone**
```cpp
#define TIME_ZONE_OFFSET  2  // UTC+2 (Paris)
```

**LCD Backlight Timeout**
```cpp
#define LCD_BACKLIGHT_TIMEOUT  30000  // 30 seconds
```

**LED Colors**
```cpp
#define COLOR_HOUR_R    0    // Red
#define COLOR_HOUR_G    0    // Green
#define COLOR_HOUR_B    127  // Blue
```

**NTP Synchronization Time**
```cpp
#define NTP_SYNC_HOUR    1   // 1 AM
#define NTP_SYNC_MINUTE  1   // at 1:01 AM
```

---

## 🔧 Troubleshooting

### Common Issues

**Clock won't connect to WiFi**
- Check `secrets.h` (correct SSID and password)
- Ensure network is 2.4 GHz (not 5 GHz)
- Check WiFi range

**DHT22 sensors return errors**
- Check connections (VCC, GND, Data)
- Respect delay between readings (minimum 2 seconds)
- Test with simple test sketch

**LCD display shows nothing**
- Check I2C address (I2C scanner if needed)
- Adjust contrast (potentiometer on module back)
- Check 5V power supply

**LEDs don't light up**
- Check power supply (sufficient 5V for all LEDs)
- Respect Data In → Data Out order
- Test with simple NeoPixel sketch

📖 **[Complete Troubleshooting Guide → docs/INSTALL.md](./docs/INSTALL.md#troubleshooting)**

---

## 📚 Documentation

- 📘 **[HARDWARE.md](./docs/HARDWARE.md)** - Hardware list, wiring diagrams, connections
- 📗 **[INSTALL.md](./docs/INSTALL.md)** - Detailed installation, libraries, configuration
- 📕 **[CHANGELOG.md](./CHANGELOG.md)** - Version history and changes
- 📄 **[LICENSE](./LICENSE)** - Complete MIT License

---

## 🤝 Contributing

Contributions are welcome!

1. Fork the project
2. Create a branch (`git checkout -b feature/improvement`)
3. Commit (`git commit -m 'Add feature'`)
4. Push (`git push origin feature/improvement`)
5. Open a Pull Request

---

## 📄 License

This project is licensed under the **MIT License** - see the [LICENSE](./LICENSE) file for details.

---

## 👤 Author

**F. Baillon**
- GitHub: [@fbaillon19](https://github.com/fbaillon19)

---

**Version:** 1.1.0  
**Last Update:** January 2025