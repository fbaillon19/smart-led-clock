# Smart LED Clock 🕐

> Horloge LED analogique connectée avec monitoring environnemental

[![Version](https://img.shields.io/badge/version-1.0.0-blue)](https://github.com/VOTRE-USERNAME/smart-led-clock/releases)
[![License](https://img.shields.io/badge/license-MIT-green)](LICENSE)
[![Arduino](https://img.shields.io/badge/Arduino-R4_WiFi-00979D)](https://docs.arduino.cc/hardware/uno-r4-wifi/)

## 📦 Fonctionnalités

- ✅ Affichage LED analogique précis (interruption matérielle 1Hz)
- ✅ Capteurs température/humidité dual (DHT22)
- ✅ Qualité de l'air (MQ135)
- ✅ Phase de lune motorisée avec calibration automatique
- ✅ Écran LCD 20×4 multi-modes avec extinction auto
- ✅ Serveur web de configuration
- ✅ Logging MQTT avec buffer RAM (16h offline)
- ✅ Synchronisation NTP quotidienne

## 🔧 Matériel requis

- Arduino Uno R4 WiFi
- DS3231 RTC module
- LCD 20×4 I2C
- 2× DHT22 temperature/humidity sensors
- MQ135 air quality sensor
- 3× NeoPixel LED strips (12/60/10 LEDs)
- Stepper motor 28BYJ-48 + ULN2003 driver
- LDR + LED pour calibration lune
- Bouton poussoir

## 🚀 Quick Start

1. **Cloner le dépôt**
```bash
   git clone https://github.com/VOTRE-USERNAME/smart-led-clock.git
   cd smart-led-clock
```

2. **Configurer**
   - Ouvrir `firmware/smart-led-clock/config.h`
   - Modifier WiFi SSID/password
   - Modifier MQTT server (optionnel)
   - Adapter les pins si nécessaire

3. **Bibliothèques Arduino requises**
   - RTClib
   - Adafruit_NeoPixel
   - DHT sensor library
   - OneButton
   - LiquidCrystal_I2C
   - WiFiS3 (inclus avec R4 WiFi)
   - PubSubClient (MQTT)
   - Stepper

4. **Téléverser**
   - Ouvrir `firmware/smart-led-clock/smart-led-clock.ino`
   - Compiler et téléverser vers Arduino R4 WiFi

## 📊 Stabilité

- ✅ **41h+ de fonctionnement continu validé**
- ✅ Zero memory leak
- ✅ RAM : 19.4KB/32KB utilisés
- ✅ Timing précis via interruption DS3231

## 📝 Documentation

_(À venir dans prochaine version)_

## 🤝 Contribution

Les contributions sont bienvenues ! N'hésitez pas à ouvrir une issue ou un pull request.

## 👤 Auteur

**F. Baillon**

## 📄 Licence

MIT License - voir [LICENSE](LICENSE)

---

**Version** : 1.0.0 | **Date** : Janvier 2025