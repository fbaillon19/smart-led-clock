# Smart LED Clock

*[Français](README_FR.md) | English*

A beautiful multi-functional clock with LED rings display, temperature monitoring, air quality sensing, and web-based configuration interface.

![Smart LED Clock](docs/images/smart-led-clock-main.jpg)

## 🌟 Features

- **Visual LED Clock**: 60 LED outer ring for minutes/seconds, 12 LED inner ring for hours
- **Temperature Monitoring**: Indoor and outdoor temperature with humidity (DHT22 sensors)
- **Air Quality Display**: Visual air quality indicator with color-coded LED bar (MQ135 sensor)
- **Web Configuration**: Easy setup and monitoring via built-in web interface
- **NTP Synchronization**: Automatic daily time sync with atomic clock servers
- **Multiple Display Modes**: Temperature, humidity, heat index, dew point, humidex
- **Hourly Animations**: Beautiful color animations every hour
- **Night Mode**: Automatic brightness adjustment
- **Button Controls**: Physical buttons for display mode switching

## 🔧 Hardware Requirements

### Essential Components (~€136 total)
- **Arduino UNO R4 WiFi** (main controller)
- **20x4 I2C LCD Display** (information display)
- **3x WS2812B LED Strips**: 60, 12, and 10 LEDs
- **2x DHT22** sensors (indoor/outdoor temperature)
- **MQ135** air quality sensor
- **2x Push buttons** + pull-up resistors
- **5V 5A Power Supply**

See [**docs/HARDWARE.md**](docs/HARDWARE.md) for complete component list and wiring diagrams.

## 🚀 Quick Start

1. **Clone the repository**
   ```bash
   git clone https://github.com/yourusername/smart-led-clock.git
   cd smart-led-clock
   ```

2. **Setup WiFi credentials**
   ```bash
   cd firmware/smart-led-clock
   cp secret.template.h secret.h
   # Edit secret.h with your WiFi credentials
   ```

3. **Install Arduino libraries** (see installation guide)
   - Adafruit NeoPixel
   - DHT sensor library
   - LiquidCrystal I2C
   - OneButton
   - NTPClient

4. **Upload firmware**
   - Open `firmware/smart-led-clock/smart-led-clock.ino` in Arduino IDE
   - Select "Arduino UNO R4 WiFi" board
   - Upload to your board

5. **Access web interface**
   - IP address will be shown on LCD after boot
   - Open `http://[IP_ADDRESS]` in your browser

## 📖 Documentation

### 🏗️ Build Guide
- [**docs/HARDWARE.md**](docs/HARDWARE.md) - Components and wiring diagrams
- [**docs/INSTALL.md**](docs/INSTALL.md) - Complete step-by-step installation guide

### 🧪 Testing
- [**testing/README.md**](testing/README.md) - Testing procedures and tools
- [**testing/hardware_tests.md**](testing/hardware_tests.md) - Hardware validation tests
- [**testing/software_tests.md**](testing/software_tests.md) - Software testing procedures

### 🔧 Development
- Individual test codes available in `testing/test_codes/`
- Troubleshooting guide in `testing/troubleshooting.md`

## 🎯 Difficulty Level

**Medium** - Requires basic Arduino knowledge and soldering skills
- Basic electronics understanding
- Soldering experience recommended
- Arduino IDE familiarity
- WiFi network setup knowledge

## 🌐 Web Interface Features

- Real-time sensor data display
- LED brightness control
- Night mode configuration
- Display mode settings
- System status monitoring
- Network configuration

## 🛠️ Project Structure

```
smart-led-clock/
├── firmware/smart-led-clock/    # Arduino firmware
├── docs/                        # Documentation
├── testing/                     # Test codes and procedures
├── web-interface/              # Web interface assets
└── README.md                   # This file
```

## 🤝 Contributing

Contributions are welcome! Please read our contributing guidelines and:

1. Fork the project
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 🐛 Issues and Support

- **Hardware issues**: Check `testing/troubleshooting.md`
- **Software bugs**: Open an issue with detailed description
- **Feature requests**: Discussions welcome in Issues section

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👨‍💻 Author

**F. Baillon** - *Initial work* - fbaillon@gmail.com

## 🙏 Acknowledgments

- Arduino community for libraries and examples
- Adafruit for excellent LED libraries
- Contributors and testers

---

⭐ **If you find this project useful, please give it a star!**