# Changelog

## [1.0.0] - 2025-01-10

### ✨ Initial Release - Production Ready

Première version stable et complète du Smart LED Clock.

#### Fonctionnalités
- Affichage LED analogique 12h/60min/10sec avec NeoPixels
- Synchronisation NTP quotidienne automatique
- Interruption matérielle SQW DS3231 pour timing précis (1Hz)
- Double capteur DHT22 (intérieur/extérieur)
- Capteur qualité de l'air MQ135
- Écran LCD 20×4 multi-modes avec extinction automatique
- Phase de lune motorisée avec calibration automatique
- Serveur web de configuration
- Logging MQTT avec buffer RAM (16h capacité offline)
- Stockage EEPROM des paramètres

#### Technique
- Protection I2C pendant opérations MQTT (flag mqttBusy)
- Élimination complète des String (prevention memory leak)
- Optimisation RAM : 19.4KB/32KB utilisés
- Buffer circulaire 200 points (2.8KB)
- Stabilité validée : 41h+ sans crash

#### Matériel supporté
- Arduino Uno R4 WiFi
- DS3231 RTC module
- LCD 20×4 I2C
- 2× DHT22
- MQ135
- 3× NeoPixel strips (12/60/10 LEDs)
- Moteur pas-à-pas 28BYJ-48
- LDR + LED calibration lune

[1.0.0]: https://github.com/VOTRE-USERNAME/smart-led-clock/releases/tag/v1.0.0