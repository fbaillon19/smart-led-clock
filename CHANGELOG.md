# Changelog

All notable changes to the Smart LED Clock project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2025-01-XX

### Added
- 🎉 **Initial stable release**
- ⏰ Analog LED clock with 12-hour and 60-minute/second rings
- 📺 LCD 20x4 display with 3 interchangeable modes:
  - Temperature & Humidity (default)
  - Feels-like temperature & Dew point
  - Humidex comfort index
- 🌡️ Dual DHT22 sensors (indoor/outdoor temperature and humidity)
- 💨 Air quality monitoring with MQ135 sensor
- 💡 10-LED bar for visual air quality display with color gradient
- 🌐 WiFi connectivity with automatic NTP time synchronization
- 🕐 DS3231 RTC module with battery backup
- 🔘 Single button control with short press (mode cycle) and long press (default mode)
- 💡 Automatic LCD backlight management (30-second timeout)
- 🎨 Hourly color animation on LED ring
- 📊 Calculated indices: heat index, dew point, humidex
- 🏗️ **Modular architecture** with separate files:
  - `config.h` - Centralized configuration
  - `rtc.h/cpp` - RTC, WiFi, and NTP management
  - `sensors.h/cpp` - DHT22 and MQ135 sensor handling
  - `display.h/cpp` - LCD display and modes
  - `leds.h/cpp` - LED rings and animations
  - `button.h/cpp` - Button input handling
- 📚 Comprehensive documentation:
  - README (bilingual FR/EN)
  - HARDWARE.md - Complete hardware guide
  - INSTALL.md - Installation instructions
  - CHANGELOG.md - Version history
  - LICENSE - MIT license

### Technical Details
- **Platform**: Arduino Uno R4 WiFi
- **Language**: C++ (Arduino framework)
- **Architecture**: Modular with header/implementation separation
- **Memory optimization**: Static buffers, efficient LCD updates
- **Power management**: LED brightness control, LCD auto-off
- **Time accuracy**: NTP sync daily at 1:01 AM + RTC backup

### Libraries Used
- Adafruit NeoPixel - LED control
- RTClib - DS3231 RTC
- LiquidCrystal I2C - LCD display
- DHT sensor library - Temperature/humidity
- WiFiS3 - WiFi connectivity
- NTPClient - Time synchronization
- OneButton - Button debouncing

---

## [Unreleased]

### Planned Features
- 🌤️ Weather forecast integration via API
- 📱 Mobile app for remote configuration
- 🔊 Audio alerts for air quality thresholds
- 📈 Historical data logging to SD card
- 🎨 Customizable LED animations
- 🌐 Web interface for configuration
- 📊 Additional display modes (calendar, alarms)

---

## Development Phases (Historical)

### Phase 5 - Button Control & Display Modes
- Added single-button control system
- Implemented 3 display modes
- Added LCD backlight auto-off
- Refined user interface

### Phase 4 - NTP Synchronization
- Integrated WiFi connectivity
- Added NTP time synchronization
- Implemented daily automatic sync
- RTC fallback mechanism

### Phase 3.2 - Air Quality Monitoring
- Added MQ135 air quality sensor
- Implemented 10-LED quality bar
- Color gradient based on AQI
- Real-time air quality display

### Phase 3.1 - Dual Temperature Sensors
- Added second DHT22 sensor (outdoor)
- Implemented indoor/outdoor comparison
- Added calculated indices (feels-like, dew point, humidex)

### Phase 2 - LCD Display
- Integrated 20x4 LCD screen
- Added I2C communication
- Implemented time and sensor data display

### Phase 1 - Core Clock
- Basic LED ring clock functionality
- RTC DS3231 integration
- Hour (12 LED) and minute/second (60 LED) rings
- Hourly animation system

---

## Version Numbering

This project follows [Semantic Versioning](https://semver.org/):
- **MAJOR** version for incompatible API/hardware changes
- **MINOR** version for new features (backward compatible)
- **PATCH** version for bug fixes and minor improvements

---

## How to Contribute

See [README.md](./README.md) for contribution guidelines.

For bug reports and feature requests, please open an issue on GitHub.

---

**Legend**:
- 🎉 Major milestone
- ⏰ Time-related feature
- 📺 Display feature
- 🌡️ Sensor feature
- 💨 Air quality feature
- 🌐 Connectivity feature
- 🔘 User interface feature
- 🏗️ Architecture change
- 📚 Documentation
- 🐛 Bug fix
- ⚡ Performance improvement
- 🔒 Security fix