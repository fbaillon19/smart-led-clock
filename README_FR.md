# Smart LED Clock - Documentation Française 🇫🇷

> Horloge intelligente à LED avec capteurs environnementaux et synchronisation NTP

[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)](https://github.com/fbaillon19/smart-led-clock)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](./LICENSE)
[![Arduino](https://img.shields.io/badge/Arduino-Uno%20R4%20WiFi-00979D.svg)](https://www.arduino.cc/)

[🇬🇧 English Version](./README_EN.md) | [📚 Documentation complète](./docs/)

---

## 🎯 Vue d'ensemble

Smart LED Clock est une horloge connectée qui combine un affichage analogique élégant sur anneaux LED avec des capteurs environnementaux et une interface de contrôle intuitive.

### Caractéristiques principales

- ⏰ **Affichage analogique LED** : Deux anneaux (12 LED heures, 60 LED minutes/secondes)
- 📺 **Écran LCD 20x4** : 3 modes d'affichage interchangeables
- 🌡️ **Capteurs environnementaux** : Température et humidité (intérieur/extérieur)
- 💨 **Qualité de l'air** : Capteur MQ135 avec barre LED colorée
- 🌐 **Connectivité WiFi** : Synchronisation NTP automatique
- 🔘 **Interface simple** : Contrôle par bouton unique
- 🎨 **Animations** : Effet lumineux à chaque heure pile

### Aperçu des modes d'affichage

**Mode 1 - Température & Humidité** (défaut)
```
03/10/2025        Sam
Heure: 14:32:45
Int:22.5°C   65%
Ext:18.3°C AQI:42
```

**Mode 2 - Températures ressenties**
```
Temp. Ressentie
Exterieur: 18.3°C
Ressenti : 17.1°C
Pt rosee : 12.4°C
```

**Mode 3 - Indice Humidex**
```
Indice Humidex
Humidex:        23
Inconfort certain
Exterieur uniquement
```

---

## ⚡ Quick Start

### 1. Matériel requis

**Composants principaux :**
- Arduino Uno R4 WiFi
- Écran LCD I2C 20x4
- 2× DHT22 (température/humidité)
- Capteur MQ135 (qualité air)
- 3× Anneaux/barres LED NeoPixel (12, 60, 10 LED)
- Module RTC DS3231
- Bouton poussoir

📋 **[Liste complète et schémas → docs/HARDWARE.md](./docs/HARDWARE.md)**

### 2. Installation

```bash
# 1. Cloner le dépôt
git clone https://github.com/fbaillon19/smart-led-clock.git
cd smart-led-clock

# 2. Créer le fichier de configuration WiFi
cp firmware/smart-led-clock/secrets.template.h firmware/smart-led-clock/secrets.h
# Éditer secrets.h avec vos identifiants WiFi

# 3. Ouvrir dans Arduino IDE
# File: firmware/smart-led-clock/smart-led-clock.ino

# 4. Installer les bibliothèques requises
# Voir docs/INSTALL.md pour la liste complète

# 5. Compiler et téléverser
```

🔧 **[Guide d'installation détaillé → docs/INSTALL.md](./docs/INSTALL.md)**

### 3. Configuration WiFi

Éditer `firmware/smart-led-clock/secrets.h` :
```cpp
const char* ssid = "VotreSSID";
const char* pass = "VotreMotDePasse";
```

### 4. Premier démarrage

1. Connecter l'Arduino au PC via USB
2. Ouvrir le moniteur série (115200 bauds)
3. Observer la séquence d'initialisation
4. L'horloge se synchronise automatiquement via NTP

---

## 🎮 Utilisation

### Contrôle par bouton

| Action | Résultat |
|--------|----------|
| **Clic court** (LCD éteint) | Rallume le rétroéclairage |
| **Clic court** (LCD allumé) | Change de mode d'affichage |
| **Appui long** (>2s) | Retour au mode par défaut |

### Fonctionnalités automatiques

- 💡 **Rétroéclairage LCD** : Extinction automatique après 30 secondes
- 🌐 **Synchronisation NTP** : Quotidienne à 1h01 du matin
- 🎨 **Animation** : À chaque heure pile (14:00, 15:00...)
- 📊 **Mise à jour capteurs** : Toutes les 2 secondes

---

## 🏗️ Architecture logicielle

Le projet utilise une **architecture modulaire** :

```
firmware/smart-led-clock/
├── smart-led-clock.ino      # Programme principal
├── config.h                 # Configuration centralisée
├── rtc.h / rtc.cpp         # RTC + WiFi + NTP
├── sensors.h / sensors.cpp  # Capteurs (DHT22, MQ135)
├── display.h / display.cpp  # LCD et modes d'affichage
├── leds.h / leds.cpp       # Anneaux LED + animations
├── button.h / button.cpp   # Gestion bouton
└── secrets.h               # WiFi (à créer)
```

### Avantages de l'architecture

✅ **Modulaire** : Chaque fonctionnalité dans son propre module  
✅ **Maintenable** : Code organisé et commenté  
✅ **Évolutif** : Facile d'ajouter de nouvelles fonctionnalités  
✅ **Testable** : Modules indépendants  

---

## ⚙️ Configuration

### Personnalisation rapide

Éditer `config.h` pour modifier :

**Fuseau horaire**
```cpp
#define TIME_ZONE_OFFSET  2  // UTC+2 (Paris)
```

**Timeout rétroéclairage LCD**
```cpp
#define LCD_BACKLIGHT_TIMEOUT  30000  // 30 secondes
```

**Couleurs des LED**
```cpp
#define COLOR_HOUR_R    0    // Rouge
#define COLOR_HOUR_G    0    // Vert
#define COLOR_HOUR_B    127  // Bleu
```

**Heure de synchronisation NTP**
```cpp
#define NTP_SYNC_HOUR    1   // 1h du matin
#define NTP_SYNC_MINUTE  1   // à 1h01
```

---

## 🔧 Dépannage

### Problèmes courants

**L'horloge ne se connecte pas au WiFi**
- Vérifier `secrets.h` (SSID et mot de passe corrects)
- S'assurer que le réseau est en 2.4 GHz (pas 5 GHz)
- Vérifier la portée WiFi

**Les capteurs DHT22 retournent des erreurs**
- Vérifier les connexions (VCC, GND, Data)
- Respecter le délai entre lectures (2 secondes minimum)
- Tester avec un sketch simple de test

**L'écran LCD n'affiche rien**
- Vérifier l'adresse I2C (scanner I2C si besoin)
- Ajuster le contraste (potentiomètre au dos du module)
- Vérifier l'alimentation 5V

**Les LED ne s'allument pas**
- Vérifier l'alimentation (5V suffisant pour tous les LED)
- Respecter l'ordre Data In → Data Out
- Tester avec un sketch NeoPixel simple

📖 **[Guide de dépannage complet → docs/INSTALL.md](./docs/INSTALL.md#dépannage)**

---

## 📚 Documentation

- 📘 **[HARDWARE.md](./docs/HARDWARE.md)** - Liste matériel, schémas de câblage, connexions
- 📗 **[INSTALL.md](./docs/INSTALL.md)** - Installation détaillée, bibliothèques, configuration
- 📕 **[CHANGELOG.md](./CHANGELOG.md)** - Historique des versions et modifications
- 📄 **[LICENSE](./LICENSE)** - Licence MIT complète

---

## 🤝 Contribution

Les contributions sont les bienvenues !

1. Fork le projet
2. Créer une branche (`git checkout -b feature/amelioration`)
3. Commiter (`git commit -m 'Ajout fonctionnalité'`)
4. Push (`git push origin feature/amelioration`)
5. Ouvrir une Pull Request

---

## 📄 Licence

Ce projet est sous licence **MIT** - voir le fichier [LICENSE](./LICENSE) pour plus de détails.

---

## 👤 Auteur

**F. Baillon**
- GitHub: [@fbaillon19](https://github.com/fbaillon19)

---

**Version :** 1.0.0  
**Dernière mise à jour :** Janvier 2025