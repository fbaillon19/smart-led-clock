# Smart LED Clock - Horloge LED Intelligente

*Français | [English](README.md)*

Une magnifique horloge multifonctionnelle avec affichage à anneaux LED, surveillance de température, détection de qualité de l'air et interface web de configuration.

![Smart LED Clock](docs/images/smart-led-clock-main.jpg)

## 🌟 Fonctionnalités

- **Horloge LED Visuelle** : Anneau externe de 60 LED pour minutes/secondes, anneau interne de 12 LED pour les heures
- **Surveillance de Température** : Température et humidité intérieure et extérieure (capteurs DHT22)
- **Affichage Qualité de l'Air** : Indicateur visuel de qualité d'air avec barre LED colorée (capteur MQ135)
- **Configuration Web** : Configuration et surveillance faciles via interface web intégrée
- **Synchronisation NTP** : Synchronisation automatique quotidienne avec serveurs d'horloge atomique
- **Modes d'Affichage Multiples** : Température, humidité, indice de chaleur, point de rosée, humidex
- **Animations Horaires** : Belles animations colorées chaque heure
- **Mode Nuit** : Ajustement automatique de la luminosité
- **Contrôles Physiques** : Boutons physiques pour changer les modes d'affichage

## 🔧 Matériel Requis

### Composants Essentiels (~136€ total)
- **Arduino UNO R4 WiFi** (contrôleur principal)
- **Afficheur LCD 20x4 I2C** (affichage des informations)
- **3x Rubans LED WS2812B** : 60, 12 et 10 LED
- **2x Capteurs DHT22** (température intérieure/extérieure)
- **Capteur MQ135** qualité de l'air
- **2x Boutons poussoirs** + résistances de pull-up
- **Alimentation 5V 5A**

Voir [**docs/HARDWARE.md**](docs/HARDWARE.md) pour la liste complète des composants et schémas de câblage.

## 🚀 Démarrage Rapide

1. **Cloner le dépôt**
   ```bash
   git clone https://github.com/yourusername/smart-led-clock.git
   cd smart-led-clock
   ```

2. **Configurer les identifiants WiFi**
   ```bash
   cd firmware/smart-led-clock
   cp secret.template.h secret.h
   # Éditer secret.h avec vos identifiants WiFi
   ```

3. **Installer les bibliothèques Arduino** (voir guide d'installation)
   - Adafruit NeoPixel
   - DHT sensor library
   - LiquidCrystal I2C
   - OneButton
   - NTPClient

4. **Téléverser le firmware**
   - Ouvrir `firmware/smart-led-clock/smart-led-clock.ino` dans l'IDE Arduino
   - Sélectionner la carte "Arduino UNO R4 WiFi"
   - Téléverser vers votre carte

5. **Accéder à l'interface web**
   - L'adresse IP sera affichée sur le LCD après démarrage
   - Ouvrir `http://[ADRESSE_IP]` dans votre navigateur

## 📖 Documentation

### 🏗️ Guide de Construction
- [**docs/HARDWARE.md**](docs/HARDWARE.md) - Composants et schémas de câblage
- [**docs/INSTALL.md**](docs/INSTALL.md) - Guide d'installation complet étape par étape

### 🧪 Tests
- [**testing/README.md**](testing/README.md) - Procédures et outils de test
- [**testing/hardware_tests.md**](testing/hardware_tests.md) - Tests de validation matérielle
- [**testing/software_tests.md**](testing/software_tests.md) - Procédures de test logiciel

### 🔧 Développement
- Codes de test individuels disponibles dans `testing/test_codes/`
- Guide de dépannage dans `testing/troubleshooting.md`

## 🎯 Niveau de Difficulté

**Moyen** - Nécessite des connaissances de base Arduino et compétences de soudure
- Compréhension de base de l'électronique
- Expérience de soudure recommandée
- Familiarité avec l'IDE Arduino
- Connaissances de configuration réseau WiFi

## 🌐 Fonctionnalités Interface Web

- Affichage en temps réel des données capteurs
- Contrôle de luminosité LED
- Configuration mode nuit
- Paramètres des modes d'affichage
- Surveillance de l'état système
- Configuration réseau

## 🛠️ Structure du Projet

```
smart-led-clock/
├── firmware/smart-led-clock/    # Firmware Arduino
├── docs/                        # Documentation
├── testing/                     # Codes de test et procédures
├── web-interface/              # Ressources interface web
└── README.md                   # Ce fichier
```

## 🤝 Contribuer

Les contributions sont les bienvenues ! Veuillez lire nos directives de contribution et :

1. Forker le projet
2. Créer une branche de fonctionnalité (`git checkout -b feature/fonctionnalite-geniale`)
3. Commiter vos changements (`git commit -m 'Ajouter fonctionnalité géniale'`)
4. Pousser vers la branche (`git push origin feature/fonctionnalite-geniale`)
5. Ouvrir une Pull Request

## 🐛 Problèmes et Support

- **Problèmes matériels** : Consultez `testing/troubleshooting.md`
- **Bugs logiciels** : Ouvrez un issue avec description détaillée
- **Demandes de fonctionnalités** : Discussions bienvenues dans la section Issues

## 📄 Licence

Ce projet est sous licence MIT - voir le fichier [LICENSE](LICENSE) pour les détails.

## 👨‍💻 Auteur

**F. Baillon** - *Travail initial* - fbaillon@gmail.com

## 🙏 Remerciements

- Communauté Arduino pour les bibliothèques et exemples
- Adafruit pour d'excellentes bibliothèques LED
- Contributeurs et testeurs

---

⭐ **Si vous trouvez ce projet utile, merci de lui donner une étoile !**

## 📋 Notes Spécifiques Françaises

### Réglementations
- **Alimentation** : Utiliser uniquement des alimentations certifiées CE
- **WiFi** : Compatible avec les fréquences 2.4GHz autorisées en France
- **Sécurité** : Respecter les normes électriques domestiques françaises

### Fournisseurs Recommandés (France)
- **Composants électroniques** : Gotronic, Conrad, RS Components
- **Arduino et shields** : Arduino.cc (distributeurs officiels)
- **Impression 3D** : Sculpteo, 3D Hubs pour boîtiers personnalisés

### Support Communautaire Francophone
- **Forum Arduino France** : arduino-france.com
- **Discord Makers France** : Communautés de makers francophones
- **FabLabs** : Espaces de fabrication collaborative près de chez vous

### Configuration Spécifique France
```cpp
// Dans config.h pour la France :
#define TIME_ZONE_OFFSET        1     // UTC+1 (heure d'hiver)
// ou
#define TIME_ZONE_OFFSET        2     // UTC+2 (heure d'été)

// Serveurs NTP français recommandés :
const char* ntp_server_primary = "0.fr.pool.ntp.org";
const char* ntp_server_secondary = "1.fr.pool.ntp.org";
```

### Unités de Mesure
- **Température** : Celsius (par défaut)
- **Dimensions** : Métriques (cm, mm)
- **Alimentation** : 230V AC → 5V DC (adaptateur secteur)

---

*Ce projet a été développé avec ❤️ en France*