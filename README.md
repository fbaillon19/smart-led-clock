# Smart LED Clock

[🇫🇷 Français](./docs/README_FR.md) | [🇬🇧 English](./docs/README_EN.md)

---

## 🇫🇷 Version Française

Une horloge intelligente à LED avec affichage analogique sur anneaux LED et affichage numérique sur écran LCD. Le projet comprend des capteurs de température/humidité intérieur/extérieur, un capteur de qualité de l'air, une synchronisation NTP via WiFi, et un contrôle par bouton.

**[Lire la documentation complète en français →](./docs/README_FR.md)**

### Caractéristiques principales
- 🕐 Affichage analogique sur anneaux LED (12h + 60min/sec)
- 📊 Surveillance température/humidité (DHT22)
- 🌡️ Indices calculés : ressenti, point de rosée, humidex
- 💨 Qualité de l'air (MQ135) avec barre LED
- 🌐 Synchronisation WiFi/NTP automatique
- 🔘 Contrôle par bouton unique (3 modes d'affichage)
- 💾 Sauvegarde temps via RTC DS3231

---

## 🇬🇧 English Version

An intelligent LED clock with analog display on LED rings and digital display on LCD screen. The project includes indoor/outdoor temperature/humidity sensors, air quality sensor, NTP synchronization via WiFi, and button control.

**[Read full documentation in English →](./docs/README_EN.md)**

### Main Features
- 🕐 Analog display on LED rings (12h + 60min/sec)
- 📊 Temperature/humidity monitoring (DHT22)
- 🌡️ Calculated indices: feels-like, dew point, humidex
- 💨 Air quality (MQ135) with LED bar
- 🌐 Automatic WiFi/NTP synchronization
- 🔘 Single button control (3 display modes)
- 💾 Time backup via DS3231 RTC

---

## Quick Start

```bash
# Clone the repository
git clone https://github.com/fbaillon19/smart-led-clock.git
cd smart-led-clock

# Open in Arduino IDE
# File: firmware/smart-led-clock/smart-led-clock.ino

# Configure WiFi credentials
# Create secrets.h with your WiFi SSID and password

# Upload to Arduino Uno R4 WiFi
```

## License

MIT License - See [LICENSE](./LICENSE) file for details.

## Author

**F. Baillon** - [GitHub](https://github.com/fbaillon19)

---

**Version:** 1.0.0 | **Last Updated:** January 2025