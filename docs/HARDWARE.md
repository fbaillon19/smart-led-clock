# Hardware Setup Guide

## 📦 Components List

### Essential Components

| Component | Quantity | Description | Estimated Price |
|-----------|----------|-------------|----------------|
| Arduino UNO R4 WiFi | 1 | Main microcontroller with WiFi | ~€35 |
| DS3231 RTC Module | 1 | Precision Real-Time Clock with battery backup | ~€5 |
| WS2812B LED Strip | 1 | 60 LEDs for minutes/seconds ring | ~€15 |
| WS2812B LED Strip | 1 | 12 LEDs for hours ring | ~€8 |
| WS2812B LED Strip | 1 | 10 LEDs for air quality bar | ~€6 |
| LCD 20x4 I2C | 1 | Display for temperature/info | ~€12 |
| DHT22 | 2 | Temperature & humidity sensors | ~€20 |
| MQ135 | 1 | Air quality sensor | ~€8 |
| Push Button | 2 | User interface | ~€2 |
| Power Supply 5V 5A | 1 | Main power supply | ~€15 |
| Breadboard/PCB | 1 | Prototyping board | ~€10 |
| Jumper Wires | 1 set | Connections | ~€5 |
| **Total** | | | **~€136** |

### Optional Components

| Component | Purpose | Price |
|-----------|---------|-------|
| BMP180 | Atmospheric pressure (weather trends) | ~€5 |
| PIR Sensor | Presence detection for night mode | ~€3 |
| SD Card Module | Data logging | ~€5 |

## 🔌 Pin Connections

### Arduino UNO R4 WiFi Pinout

```
Digital Pins:
├── D2  → Button 1 (+ pull-up resistor)
├── D3  → Button 2 (+ pull-up resistor)
├── D6  → DHT22 Indoor (Data)
├── D7  → DHT22 Outdoor (Data)
├── D9  → WS2812B Minutes/Seconds (PWM)
├── D10 → WS2812B Hours (PWM)
└── D11 → WS2812B Air Quality (PWM)

Analog Pins:
└── A0  → MQ135 Air Quality Sensor

I2C Pins (for LCD):
├── SDA → LCD SDA
└── SCL → LCD SCL

Power:
├── 5V  → All components VCC
├── 3.3V → (unused)
└── GND → Common ground
```

## 🔧 Wiring Diagram

### Basic Connections

#### LED Strips (WS2812B)
```
LED Strip → Arduino
VCC (Red)   → 5V
GND (Black) → GND  
DIN (White) → Digital Pin (D9/D10/D11)
```

#### DHT22 Sensors
```
DHT22 → Arduino
VCC → 5V
GND → GND
DATA → Digital Pin (D6 or D7)
```

#### LCD Display (I2C)
```
LCD → Arduino
VCC → 5V
GND → GND
SDA → SDA (A4)
SCL → SCL (A5)
```

#### DS3231 RTC Module (I2C)
```
DS3231 → Arduino
VCC → 5V (or 3.3V)
GND → GND
SDA → SDA (A4) - shared with LCD
SCL → SCL (A5) - shared with LCD
```

**Note**: Both LCD and DS3231 share the same I2C bus. The DS3231 includes a coin cell battery for timekeeping during power loss.

#### MQ135 Air Quality Sensor
```
MQ135 → Arduino
VCC → 5V
GND → GND
AOUT → A0
```

#### Push Buttons
```
Button → Arduino
One leg → Digital Pin (D2 or D3)
Other leg → GND
Note: Use internal pull-up resistors (INPUT_PULLUP)
```

## ⚡ Power Requirements

- **Total current consumption**: ~3A (estimated)
- **Power supply**: 5V 5A recommended
- **LED strips**: ~2.5A (at full brightness)
- **Arduino + sensors**: ~0.5A

## 📐 Mechanical Assembly

### Suggested Layout
```
    ┌─────────────────────────────┐
    │     30cm Diameter Plate     │
    │  ┌─────────────────────┐    │
    │  │   60 LED Ring       │    │
    │  │  ┌─────────────┐    │    │
    │  │  │ 12 LED Ring │    │    │
    │  │  │   ┌─────┐   │    │    │
    │  │  │   │ LCD │   │    │    │
    │  │  │   └─────┘   │    │    │
    │  │  └─────────────┘    │    │
    │  │    10 LED Bar       │    │
    │  │    [Buttons]        │    │
    │  └─────────────────────┘    │
    └─────────────────────────────┘
```

## 🛠️ Required Tools

- Soldering iron and solder
- Wire strippers
- Multimeter
- Hot glue gun (for assembly)
- Drill (for mounting holes)

## ⚠️ Safety Notes

- Always disconnect power when making connections
- Check all connections before powering on
- Use appropriate gauge wire for power connections
- Ensure proper ventilation for air quality sensor
- Keep outdoor sensor weatherproof

## 🧪 Testing Checklist

- [ ] Power supply provides stable 5V
- [ ] All LED strips light up
- [ ] LCD displays correctly
- [ ] Both DHT22 sensors read temperature
- [ ] Air quality sensor provides readings
- [ ] Buttons respond properly
- [ ] WiFi connection established

## 📚 Arduino Libraries Required

### Install Required Libraries
In Arduino IDE, go to **Tools > Manage Libraries** and install:

```
Search and Install:
├── "Adafruit NeoPixel" by Adafruit
├── "DHT sensor library" by Adafruit  
├── "LiquidCrystal I2C" by Marcin Saj (version 1.1.4 or later)
├── "OneButton" by Matthias Hertel
├── "RTClib" by Adafruit (for DS3231 RTC module)
└── "NTPClient" by Fabrice Weinberg
```

**Important for Arduino UNO R4:**
- Use "LiquidCrystal I2C" by **Marcin Saj** (not Frank de Brabander) - better R4 compatibility
- If you still have compilation issues, try "New LiquidCrystal" library instead
- WiFiS3 library is built into the Arduino UNO R4 core

### Alternative LCD Library (if needed)
If you encounter issues with LiquidCrystal_I2C, use this alternative:
```cpp
// Replace in code:
// #include <LiquidCrystal_I2C.h>
// with:
#include <LiquidCrystal.h>  // Standard library
// And use parallel connection instead of I2C
```

## 🔍 Troubleshooting

### Common Issues

**LEDs not lighting:**
- Check power supply capacity
- Verify data pin connections
- Ensure common ground

**Temperature readings show NaN:**
- Check DHT22 wiring
- Verify power supply stability
- Add small delay after sensor initialization

**WiFi not connecting:**
- Check credentials in secrets.h
- Verify WiFi signal strength
- Try different WiFi channels

**LCD not displaying:**
- Check I2C address (usually 0x27)
- Verify SDA/SCL connections
- Test with I2C scanner sketch