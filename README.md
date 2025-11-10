# Smart LED Clock 🕐

> A connected multi-functional analog LED clock with environmental monitoring

[![Version](https://img.shields.io/badge/version-1.0.0-blue)](https://github.com/fbaillon19/smart-led-clock/releases)
[![License](https://img.shields.io/badge/license-MIT-green)](LICENSE)
[![Arduino](https://img.shields.io/badge/Arduino-R4_WiFi-00979D)](https://docs.arduino.cc/hardware/uno-r4-wifi/)

## 📖 Overview

The Smart LED Clock is an Arduino-based analog clock that uses LED rings to display time and includes comprehensive environmental monitoring capabilities. The project combines precise timekeeping with temperature, humidity, and air quality sensors, all displayed on an LCD screen and accessible via a web interface.

**Key Features:**
- Analog LED time display using NeoPixel rings (60 LEDs for minutes/seconds, 12 LEDs for hours)
- Dual DHT22 temperature/humidity sensors (indoor & outdoor)
- MQ135 air quality monitoring with LED bar visualization
- LCD 20×4 display with multiple modes and automatic backlight
- Moon phase display using stepper motor
- WiFi connectivity with web interface
- MQTT data logging with offline buffer
- Daily NTP synchronization
- EEPROM configuration storage

## 🎯 Target Audience

This project is designed for makers with:
- Basic Arduino programming knowledge
- Understanding of electronics and component wiring
- Familiarity with Arduino IDE and library management

## 📦 Hardware Requirements

### Main Components

| Component | Specification | Quantity | Notes |
|-----------|--------------|----------|-------|
| Microcontroller | Arduino Uno R4 WiFi | 1 | Core controller with WiFi |
| RTC Module | DS3231 | 1 | Precise timekeeping with 1Hz interrupt |
| Display | LCD 20×4 I2C | 1 | Multi-mode information display |
| Temperature Sensors | DHT22 | 2 | Indoor & outdoor monitoring |
| Air Quality Sensor | MQ135 | 1 | Analog air quality measurement |
| LED Ring (RUB1) | WS2812B 60 LEDs | 1 | Minutes/seconds display |
| LED Ring (RUB2) | WS2812B 12 LEDs | 1 | Hours display |
| LED Bar (RUB3) | WS2812B 10 LEDs | 1 | Air quality visualization |
| Stepper Motor | 28BYJ-48 + ULN2003 | 1 | Moon phase indicator |
| Push Button | Standard momentary | 1 | Mode switching |
| Power Supply | 5V DC | 1 | Adequate current for LEDs |

### Physical Design

The clock features a circular platform (approximately 30cm diameter) with:
- **Outer ring (RUB1):** 60 multicolor LEDs displaying minutes and seconds
- **Inner ring (RUB2):** 12 multicolor LEDs displaying hours  
- **Center:** LCD 20×4 display
- **Below LCD:** 10-LED bar (RUB3) for air quality indication
- **Above clock:** bi-color sphere for moon phase indication

## ⚙️ Software Requirements

### Arduino IDE
- Arduino IDE 2.x or later
- Board: Arduino Uno R4 WiFi

### Required Libraries

Install via Arduino Library Manager:

```
- RTClib (Adafruit)
- Adafruit_NeoPixel
- DHT sensor library (Adafruit)
- OneButton
- LiquidCrystal_I2C
- WiFiS3 (included with R4 WiFi core)
- PubSubClient (for MQTT)
- Stepper (Arduino built-in)
```

## 🚀 Quick Start

### 1. Clone Repository

```bash
git clone https://github.com/fbaillon19/smart-led-clock.git
cd smart-led-clock
```

### 2. Configure Settings

Create `secrets.h` from template:

```bash
cp firmware/smart-led-clock/secrets.h.template firmware/smart-led-clock/secrets.h
```

Edit `secrets.h` with your credentials:
- WiFi SSID and password
- MQTT broker details (optional)

Review and adjust `config.h` for:
- Pin assignments
- LED colors and brightness
- Timezone offset
- NTP server settings

### 3. Install Libraries

Open Arduino IDE and install all required libraries listed above.

### 4. Hardware Setup

Connect components according to pin definitions in `config.h`:
- Default pin assignments are optimized for Arduino R4 WiFi
- Refer to [Hardware Documentation](docs/HARDWARE.md) for detailed wiring

### 5. Upload Firmware

1. Open `firmware/smart-led-clock/smart-led-clock.ino`
2. Select board: **Arduino Uno R4 WiFi**
3. Select correct COM port
4. Click Upload

### 6. First Boot

On first boot, the clock will:
1. Initialize all hardware modules
2. Connect to WiFi
3. Synchronize time via NTP
4. Display startup messages on LCD
5. Begin normal operation

## 📚 Documentation

Complete documentation is available in organized sections:

- **[Hardware Setup](docs/HARDWARE.md)** - Wiring diagrams and connections
- **[Software Architecture](docs/SOFTWARE.md)** - Code structure and modules
- **[Configuration Guide](docs/CONFIGURATION.md)** - Detailed settings reference
- **[Web Interface](docs/WEBSERVER.md)** - Web dashboard and API
- **[Testing Procedures](testing/README.md)** - Module testing guide

## 🎨 Features in Detail

### Time Display
- **LED Rings:** Analog clock face using color-coded LEDs
  - Minutes: One color on 60-LED ring
  - Seconds: Different color on same ring
  - Overlap: Third color when minute and second coincide
  - Hours: Separate color on 12-LED ring
- **Hourly Animations:** 5-second LED animation marks each hour
- **Precision:** Hardware interrupt from DS3231 ensures accurate 1Hz timing

### Environmental Monitoring
- **Indoor Temperature/Humidity:** DHT22 sensor readings
- **Outdoor Temperature/Humidity:** Second DHT22 for comparison
- **Computed Values:** Feels-like temperature, dew point, humidex
- **Air Quality:** MQ135 sensor with color-coded LED bar display

### LCD Display Modes
Cycle through modes using push button:
1. Temperature and humidity
2. Feels-like and dew point
3. Humidex comfort index

### Connectivity
- **WiFi:** Automatic connection and reconnection
- **NTP Sync:** Daily time synchronization (default: 01:01 AM)
- **Web Interface:** View data and configure settings via browser
- **MQTT Logging:** Optional data logging with 16-hour offline buffer

### Configuration Storage
- EEPROM persistence for all settings
- Wear-leveling (only writes when values change)
- Expected lifetime: 274+ years at 1 write/day

## 💡 System Specifications

### Performance
- **Memory Usage:** 19.4KB / 32KB RAM (60%)
- **Stability:** Zero memory leaks, optimized buffer management
- **Timing Accuracy:** Hardware interrupt-driven (DS3231 SQW pin)

### Power Consumption
- Varies with LED brightness settings
- Recommend 5V power supply with adequate current for all LEDs at full brightness
- Typical: 2-3A depending on LED count and brightness

## 🔧 Testing

The project includes comprehensive test programs for individual module validation:

```
testing/
├── test_arduino/         # Basic Arduino functionality
├── test_led/             # LED strip tests
├── test_lcd/             # LCD display tests  
├── test_button/          # Button input tests
├── test_rtc/             # RTC module tests
├── test_sensors/         # DHT22 and MQ135 tests
└── test_moon_phase/      # Stepper motor tests
```

**Testing Workflow:**
1. Test modules individually following the order above
2. Verify each component before proceeding
3. Refer to [Testing Guide](testing/README.md) for detailed procedures

## 🌐 Web Interface

Access the web interface at `http://<arduino-ip-address>` to:
- View real-time sensor data
- Monitor system status
- Configure clock settings
- Check MQTT logging statistics
- Control moon phase calibration

## 🤝 Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Open a Pull Request

## 🐛 Troubleshooting

Common issues and solutions:

**Clock not keeping time:**
- Verify DS3231 module connections (SDA, SCL, SQW)
- Check RTC battery if equipped
- Ensure NTP synchronization is working (check serial monitor)

**WiFi not connecting:**
- Verify credentials in `secrets.h`
- Check WiFi signal strength
- Review serial monitor for error messages

**LEDs not displaying correctly:**
- Check power supply capacity
- Verify LED strip connections and pin assignments
- Test with reduced brightness

For more help, open an issue on GitHub or consult the detailed documentation.

## 📄 License

This project is licensed under the MIT License - see [LICENSE](LICENSE) file for details.

## 👤 Author

**F. Baillon**

## 🙏 Acknowledgments

- Documentation created with assistance from Claude AI (Anthropic)
- Arduino and open-source community for excellent libraries
- All contributors and testers

## 📊 Project Status

- ✅ **Version 1.0.0** - Production ready
- ✅ Fully tested and validated
- ✅ Complete documentation
- 🔄 Ongoing maintenance and improvements

---

**Last Updated:** November 2025  
**Project URL:** https://github.com/fbaillon19/smart-led-clock