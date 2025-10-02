# Smart LED Clock

A beautiful multi-functional clock with LED rings display, temperature monitoring, air quality sensing, and web-based configuration.

## 🌟 Features

- **Visual LED Clock**: 60 LED outer ring for minutes/seconds, 12 LED inner ring for hours
- **Temperature Monitoring**: Indoor and outdoor temperature with humidity
- **Air Quality Display**: Visual air quality indicator with color-coded LED bar
- **Web Configuration**: Easy setup via built-in web interface
- **NTP Synchronization**: Automatic time sync with atomic clock servers
- **Multiple Display Modes**: Temperature, humidity, heat index, dew point, humidex
- **Hourly Animations**: Beautiful color animations every hour

## 🔧 Hardware Requirements

### Main Components
- **Arduino UNO R4 WiFi** (main controller)
- **20x4 LCD Display with I2C** (information display)
- **3x WS2812B LED Strips**:
  - 60 LEDs for minutes/seconds ring
  - 12 LEDs for hours ring  
  - 10 LEDs for air quality bar
- **2x DHT22** sensors (indoor/outdoor temperature)
- **MQ135** air quality sensor
- **2x Push buttons** for display control
- **5V 5A Power Supply**

### Optional Components
- **BMP180** pressure sensor (planned for weather trends)

## 🚀 Quick Start

1. **Clone the repository**
   ```bash
   git clone https://github.com/yourusername/smart-led-clock.git
   cd smart-led-clock
   ```

2. **Setup WiFi credentials**
   ```bash
   cp secrets.template.h secrets.h
   # Edit secrets.h with your WiFi credentials
   ```

3. **Install Arduino libraries** (see HARDWARE.md for complete list)

4. **Upload to Arduino UNO R4 WiFi**

5. **Connect to web interface** (IP shown on LCD after boot)

## 📖 Documentation

- [**HARDWARE.md**](HARDWARE.md) - Wiring diagrams and component details
- [**docs/INSTALL.md**](docs/INSTALL.md) - Step-by-step installation guide

## 🎯 Difficulty Level

**Medium** - Requires basic Arduino knowledge and soldering skills

## 🤝 Contributing

This project welcomes contributions! See issues for planned features and improvements.

## 📄 License

This project is licensed under the GNU General Public License v3.0 - see the LICENSE file for details.

## 👨‍💻 Author

**F. Baillon** - *Initial work* - fbaillon@gmail.com

---

⭐ If you find this project useful, please give it a star!