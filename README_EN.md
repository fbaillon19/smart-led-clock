# Smart LED Clock - English Documentation 🇬🇧

> Intelligent LED clock with environmental sensors and NTP synchronization

[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)](https://github.com/fbaillon19/smart-led-clock)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](./LICENSE)
[![Arduino](https://img.shields.io/badge/Arduino-Uno%20R4%20WiFi-00979D.svg)](https://www.arduino.cc/)

[🇫🇷 Version Française](./README_FR.md) | [📚 Full Documentation](./docs/)

---

## 🎯 Overview

Smart LED Clock is a connected clock that combines an elegant analog display on LED rings with environmental sensors and an intuitive control interface.

### Key Features

- ⏰ **Analog LED Display**: Two rings (12 LED hours, 60 LED minutes/seconds)
- 📺 **20x4 LCD Screen**: 3 interchangeable display modes
- 🌡️ **Environmental Sensors**: Temperature and humidity (indoor/outdoor)
- 💨 **Air Quality**: MQ135 sensor with colored LED bar
- 🌐 **WiFi Connectivity**: Automatic NTP synchronization
- 🔘 **Simple Interface**: Single button control
- 🎨 **Animations**: Light effect every hour

### Display Modes Preview

**Mode 1 - Temperature & Humidity** (default)
```
03/10/2025        Sat
Time: 14:32:45
Int:22.5°C   65%
Ext:18.3°C AQI:42
```

**Mode 2 - Feels-like Temperature**
```
Feels-like Temp
Outdoor  : 18.3°C
Feels-like: 17.1°C
Dew point: 12.4°C
```

**Mode 3 - Humidex Index**
```
Humidex Index
Humidex:        23
Some discomfort
Outdoor only
```

---

## ⚡ Quick Start

### 1. Required Hardware

**Main Components:**
- Arduino Uno R4 WiFi
- I2C LCD 20x4 Screen
- 2× DHT22 (temperature/humidity)
- MQ135 Sensor (air quality)
- 3× NeoPixel LED rings/bars (12, 60, 10 LEDs)
- DS3231 RTC Module
- Push button

📋 **[Complete list and wiring diagrams → docs/HARDWARE.md](./docs/HARDWARE.md)**

### 2. Installation

```bash
# 1. Clone the repository
git clone https://github.com/fbaillon19/smart-led-clock.git
cd smart-led-clock

# 2. Create WiFi configuration file
cp firmware/smart-led-clock/secrets.template.h firmware/smart-led-clock/secrets.h
# Edit secrets.h with your WiFi credentials

# 3. Open in Arduino IDE
# File: firmware/smart-led-clock/smart-led-clock.ino

# 4. Install required libraries
# See docs/INSTALL.md for complete list

# 5. Compile and upload
```

🔧 **[Detailed installation guide → docs/INSTALL.md](./docs/INSTALL.md)**

### 3. WiFi Configuration

Edit `firmware/smart-led-clock/secrets.h`:
```cpp
const char* ssid = "YourSSID";
const char* pass = "YourPassword";
```

### 4. First Boot

1. Connect Arduino to PC via USB
2. Open serial monitor (115200 baud)
3. Watch initialization sequence
4. Clock automatically synchronizes via NTP

---

## 🎮 Usage

### Button Control

| Action | Result |
|--------|--------|
| **Short press** (LCD off) | Turn on backlight |
| **Short press** (LCD on) | Change display mode |
| **Long press** (>2s) | Return to default mode |

### Automatic Features

- 💡 **LCD Backlight**: Auto-off after 30 seconds
- 🌐 **NTP Sync**: Daily at 1:01 AM
- 🎨 **Animation**: Every hour on the hour (14:00, 15:00...)
- 📊 **Sensor Update**: Every 2 seconds

---

## 🏗️ Software Architecture

The project uses a **modular architecture**:

```
firmware/smart-led-clock/
├── smart-led-clock.ino      # Main program
├── config.h                 # Centralized configuration
├── rtc.h / rtc.cpp         # RTC + WiFi + NTP
├── sensors.h / sensors.cpp  # Sensors (DHT22, MQ135)
├── display.h / display.cpp  # LCD and display modes
├── leds.h / leds.cpp       # LED rings + animations
├── button.h / button.cpp   # Button handling
└── secrets.h               # WiFi credentials (create this)
```

### Architecture Benefits

✅ **Modular**: Each feature in its own module  
✅ **Maintainable**: Organized and commented code  
✅ **Scalable**: Easy to add new features  
✅ **Testable**: Independent modules  

---

## ⚙️ Configuration

### Quick Customization

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

**NTP Sync Time**
```cpp
#define NTP_SYNC_HOUR    1   // 1 AM
#define NTP_SYNC_MINUTE  1   // at 1:01
```

---

## 🔧 Troubleshooting

### Common Issues

**Clock doesn't connect to WiFi**
- Check `secrets.h` (correct SSID and password)
- Ensure network is 2.4 GHz (not 5 GHz)
- Verify WiFi range

**DHT22 sensors return errors**
- Check connections (VCC, GND, Data)
- Respect delay between readings (2 seconds minimum)
- Test with simple test sketch

**LCD screen displays nothing**
- Check I2C address (use I2C scanner if needed)
- Adjust contrast (potentiometer on back of module)
- Verify 5V power supply

**LEDs don't light up**
- Check power supply (sufficient 5V for all LEDs)
- Respect Data In → Data Out order
- Test with simple NeoPixel sketch

📖 **[Complete troubleshooting guide → docs/INSTALL.md](./docs/INSTALL.md#troubleshooting)**

---

## 📚 Documentation

- 📘 **[HARDWARE.md](./docs/HARDWARE.md)** - Hardware list, wiring diagrams, connections
- 📗 **[INSTALL.md](./docs/INSTALL.md)** - Detailed installation, libraries, configuration
- 📕 **[CHANGELOG.md](./CHANGELOG.md)** - Version history and changes
- 📄 **[LICENSE](./LICENSE)** - Full MIT license

---

## 🤝 Contributing

Contributions are welcome!

1. Fork the project
2. Create a branch (`git checkout -b feature/improvement`)
3. Commit changes (`git commit -m 'Add feature'`)
4. Push to branch (`git push origin feature/improvement`)
5. Open a Pull Request

---

## 📄 License

This project is licensed under the **MIT License** - see the [LICENSE](./LICENSE) file for details.

---

## 👤 Author

**F. Baillon**
- GitHub: [@fbaillon19](https://github.com/fbaillon19)

---

**Version:** 1.0.0  
**Last Updated:** January 2025