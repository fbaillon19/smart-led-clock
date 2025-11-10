# Hardware Setup Guide

This document provides detailed information about the hardware components, wiring, and physical assembly of the Smart LED Clock.

## Table of Contents

- [Components List](#components-list)
- [Pin Connections](#pin-connections)
- [Wiring Diagram](#wiring-diagram)
- [I2C Devices](#i2c-devices)
- [Power Requirements](#power-requirements)
- [Physical Assembly](#physical-assembly)
- [Hardware Testing](#hardware-testing)
- [Troubleshooting](#troubleshooting)

## Components List

### Required Components

| Qty | Component | Specification | Purpose | Notes |
|-----|-----------|--------------|---------|-------|
| 1 | Arduino Uno R4 WiFi | Microcontroller | Main controller | Must be R4 WiFi for WiFi features |
| 1 | DS3231 RTC Module | Real-Time Clock | Precise timekeeping | Includes battery backup |
| 1 | LCD Display | 20×4 I2C | Information display | I2C address usually 0x27 or 0x3F |
| 2 | DHT22 Sensor | Temperature/Humidity | Environmental monitoring | One indoor, one outdoor |
| 1 | MQ135 Sensor | Air Quality | Air quality measurement | Requires 24-48h burn-in time |
| 1 | LED Strip RUB1 | WS2812B 60 LEDs | Minutes/seconds ring | 5V, individual addressable |
| 1 | LED Strip RUB2 | WS2812B 12 LEDs | Hours ring | 5V, individual addressable |
| 1 | LED Strip RUB3 | WS2812B 10 LEDs | Air quality bar | 5V, individual addressable |
| 1 | Stepper Motor | 28BYJ-48 | Moon phase indicator | With ULN2003 driver board |
| 1 | LDR | Photoresistor | Moon calibration sensor | Any standard LDR |
| 1 | LED | Standard LED | Calibration indicator | Any color, with current-limiting resistor |
| 1 | Push Button | Momentary switch | Mode selection | With pull-up resistor (optional) |
| 1 | Power Supply | 5V DC, 3-5A | System power | Adequate for all LEDs at full brightness |
| - | Resistors | Various | Pull-up/current-limiting | See specific circuits |
| - | Wires | Various | Connections | Dupont wires, breadboard, or PCB |

### Optional Components

- Enclosure or housing for the electronics
- 30cm circular platform for LED arrangement
- Mounting hardware
- Cable management accessories
- Heat sinks (if needed for voltage regulators)

## Pin Connections

### Arduino Uno R4 WiFi Pin Assignment

All pin definitions are configured in `firmware/smart-led-clock/config.h` and can be modified if needed.

#### Digital Pins

| Pin | Function | Component | Signal Type | Notes |
|-----|----------|-----------|-------------|-------|
| D2 | SQW Interrupt | DS3231 | Input (PULLUP) | 1Hz square wave interrupt |
| D3 | Stepper IN1 | ULN2003 | Output | Stepper motor control |
| D4 | Stepper IN2 | ULN2003 | Output | Stepper motor control |
| D5 | DHT Indoor | DHT22 | OneWire | Indoor temperature/humidity |
| D6 | DHT Outdoor | DHT22 | OneWire | Outdoor temperature/humidity |
| D7 | Stepper IN3 | ULN2003 | Output | Stepper motor control |
| D8 | Stepper IN4 | ULN2003 | Output | Stepper motor control |
| D9 | LED Minutes/Seconds | WS2812B (60) | Output (PWM) | RUB1 data line |
| D10 | LED Hours | WS2812B (12) | Output (PWM) | RUB2 data line |
| D11 | LED Air Quality | WS2812B (10) | Output (PWM) | RUB3 data line |
| D12 | Calibration LED | LED | Output | Moon phase calibration indicator |
| D13 | Push Button | Button | Input (PULLUP) | Mode switching |

#### Analog Pins

| Pin | Function | Component | Signal Type | Notes |
|-----|----------|-----------|-------------|-------|
| A0 | Air Quality | MQ135 | Analog Input | 0-1023 range |
| A1 | LDR Sensor | Photoresistor | Analog Input | Moon phase detection |
| A4 | I2C SDA | DS3231, LCD | I2C Data | Shared I2C bus |
| A5 | I2C SCL | DS3231, LCD | I2C Clock | Shared I2C bus |

#### Power Pins

| Pin | Function | Notes |
|-----|----------|-------|
| 5V | Main power output | Powers sensors and LEDs (use external supply for LEDs) |
| 3.3V | Alternative power | Can power DS3231 and DHT22 if needed |
| GND | Common ground | Must be common for all components |
| VIN | External power input | 7-12V DC if not using USB |

## Wiring Diagram

### DS3231 RTC Module

```
DS3231 Module → Arduino R4 WiFi
────────────────────────────────
VCC → 5V (or 3.3V)
GND → GND
SDA → A4 (I2C Data)
SCL → A5 (I2C Clock)
SQW → D2 (Interrupt pin)
```

**Important Notes:**
- Connect battery to DS3231 for time persistence during power loss
- SQW pin must be connected for hardware interrupt timing
- I2C pull-up resistors (4.7kΩ) are usually included on module

### LCD 20×4 I2C Display

```
LCD Module → Arduino R4 WiFi
────────────────────────────────
VCC → 5V
GND → GND
SDA → A4 (I2C Data, shared with DS3231)
SCL → A5 (I2C Clock, shared with DS3231)
```

**Important Notes:**
- Check I2C address with I2C scanner (usually 0x27 or 0x3F)
- Adjust contrast potentiometer on back of module if needed
- Shares I2C bus with DS3231 (no conflict)

### DHT22 Temperature/Humidity Sensors

```
DHT22 Indoor → Arduino R4 WiFi
────────────────────────────────
VCC (Pin 1) → 5V (or 3.3V)
DATA (Pin 2) → D5
NC (Pin 3) → Not connected
GND (Pin 4) → GND
+ Pull-up resistor 10kΩ between VCC and DATA

DHT22 Outdoor → Arduino R4 WiFi
────────────────────────────────
VCC (Pin 1) → 5V (or 3.3V)
DATA (Pin 2) → D6
NC (Pin 3) → Not connected
GND (Pin 4) → GND
+ Pull-up resistor 10kΩ between VCC and DATA
```

**Important Notes:**
- 10kΩ pull-up resistor is REQUIRED for reliable operation
- Keep sensor leads as short as practical (< 20m)
- For outdoor sensor, use weatherproof housing
- Allow 2 seconds between readings minimum

### MQ135 Air Quality Sensor

```
MQ135 Module → Arduino R4 WiFi
────────────────────────────────
VCC → 5V
GND → GND
A0 (Analog) → A0
D0 (Digital) → Not connected
```

**Important Notes:**
- Requires 24-48 hour burn-in period for accurate readings
- Sensor gets warm during operation (normal)
- Place in well-ventilated area for best results
- Analog output varies with air quality (0-1023)

### WS2812B LED Strips

```
LED Strip RUB1 (60 LEDs - Minutes/Seconds)
────────────────────────────────────────────
5V → External 5V power supply (3-5A)
GND → Common ground with Arduino
DIN → D9

LED Strip RUB2 (12 LEDs - Hours)
────────────────────────────────
5V → External 5V power supply (shared)
GND → Common ground with Arduino
DIN → D10

LED Strip RUB3 (10 LEDs - Air Quality)
────────────────────────────────────────
5V → External 5V power supply (shared)
GND → Common ground with Arduino
DIN → D11
```

**CRITICAL NOTES:**
- **Never power LEDs from Arduino's 5V pin** - Use external power supply
- All grounds must be connected (Arduino + LEDs + Power supply)
- Add 470Ω resistor in series with each DIN line (optional but recommended)
- Add 1000µF capacitor between 5V and GND at LED power input
- Estimate power: ~60mA per LED at full white brightness
  - 60 LEDs: up to 3.6A
  - 12 LEDs: up to 0.72A
  - 10 LEDs: up to 0.6A
  - Total max: ~5A (use 5A power supply for safety margin)

### Stepper Motor (28BYJ-48 with ULN2003)

```
ULN2003 Driver → Arduino R4 WiFi
────────────────────────────────────
IN1 → D3
IN2 → D4
IN3 → D7
IN4 → D8
+ (VCC) → 5V
- (GND) → GND

28BYJ-48 Motor → ULN2003 Driver
────────────────────────────────────
Use 5-pin connector (already wired in module)
```

**Important Notes:**
- ULN2003 driver board handles motor current
- Motor draws up to 240mA (acceptable from Arduino 5V)
- Ensure correct connection sequence (IN1-IN2-IN3-IN4)

### Moon Phase Calibration Components

```
LDR Photoresistor Circuit
────────────────────────────────────
LDR → A1 and 5V
10kΩ resistor → A1 and GND
(Voltage divider configuration)

Calibration LED
────────────────────────────────────
D12 → 220Ω resistor → LED anode (+)
LED cathode (-) → GND
```

**Important Notes:**
- LDR detects alignment hole in moon phase disk
- Calibration LED provides visual feedback during setup
- Voltage divider provides 0-1023 reading range

### Push Button

```
Push Button → Arduino R4 WiFi
────────────────────────────────────
One terminal → D13
Other terminal → GND
(Internal pull-up resistor enabled in software)
```

**Alternative with External Pull-up:**
```
One terminal → D13 and 10kΩ resistor to 5V
Other terminal → GND
```

## I2C Devices

The Smart LED Clock uses two I2C devices on the shared bus:

| Device | Default Address | Configurable | Purpose |
|--------|----------------|--------------|---------|
| DS3231 RTC | 0x68 | No | Real-time clock |
| LCD 20×4 | 0x27 or 0x3F | Yes (jumpers) | Display |

### I2C Address Detection

If unsure about LCD address, use the I2C scanner test program:
```bash
testing/test_codes/test_arduino/test_arduino.ino
```

This will scan and display all I2C devices found on the bus.

### I2C Pull-up Resistors

- Most I2C modules include onboard pull-up resistors (4.7kΩ)
- If using breakout boards without resistors, add 4.7kΩ resistors:
  - SDA to 5V
  - SCL to 5V

## Power Requirements

### Power Budget Calculation

| Component | Voltage | Current (typ) | Current (max) | Notes |
|-----------|---------|---------------|---------------|-------|
| Arduino R4 WiFi | 5V | 200mA | 500mA | During WiFi transmission |
| DS3231 | 5V | 0.1mA | 1mA | Very low power |
| LCD 20×4 | 5V | 20mA | 50mA | With backlight |
| DHT22 (×2) | 5V | 2mA | 5mA | During measurement |
| MQ135 | 5V | 150mA | 180mA | Heater element |
| Stepper Motor | 5V | 50mA | 240mA | When stepping |
| LED 60 (RUB1) | 5V | 600mA | 3600mA | At full brightness |
| LED 12 (RUB2) | 5V | 120mA | 720mA | At full brightness |
| LED 10 (RUB3) | 5V | 100mA | 600mA | At full brightness |
| **Total** | **5V** | **~1.2A** | **~5.9A** | See notes below |

**Notes:**
- Maximum current assumes all LEDs at full white brightness (rare)
- Typical operation: 1-2A with moderate LED brightness
- Recommend 5V 5A power supply for safety margin
- Arduino can be powered via USB or VIN (7-12V)

### Power Supply Recommendations

**Option 1: Single 5V Supply (Recommended)**
- Use quality 5V 5A switching power supply
- Connect to breadboard power rails
- Feed Arduino via 5V pin (bypass voltage regulator)
- Power all components from same supply

**Option 2: Dual Supply**
- 5V 5A supply for LEDs
- USB power for Arduino (development/testing)
- Ensure common ground connection

**CRITICAL: Common Ground**
- All components MUST share common ground
- Separate power supplies MUST have grounds connected
- Failure to connect grounds causes erratic behavior

## Physical Assembly

### Platform Design

The clock is designed around a circular platform (~30cm diameter):

```
            
                 ┌─────────────────┐
                 │   Moon Phase    │
                 │  (Top/Center)   │
                 └──────┼──────────┘
                        │
      RUB1 (60 LEDs)
  Minute/Second Ring  ●●●●●
                ●●             ●●
            ●                      ●
          ●             ●             ●
        ●        ●             ●        ●
      ●                                   ●
      ●            RUB2 (12)              ●
    ●       ●      Hour Ring        ●       ●
    ●                                       ●           
   ●                                         ●
   ●             ┌──────────────┐            ●
   ●     ●       │  LCD 20x4    │       ●    ●
   ●             └──────────────┘            ●
   ●                ●●●●●●●●●●               ●
    ●                RUB3 (10)              ●
    ●       ●                       ●       ●
      ●                                   ●
      ●                                   ●
        ●        ●              ●       ●
          ●             ●             ●
            ●●                     ●
                ●             ●●
                      ●●●●●


    [Electronics compartment below platform]
```

### LED Ring Arrangement

1. **RUB1 (60 LEDs):** Outer ring, evenly spaced
   - LED 0 at 12 o'clock position
   - Clockwise numbering
   - Displays minutes (blue) and seconds (green)

2. **RUB2 (12 LEDs):** Inner ring, evenly spaced
   - LED 0 at 12 o'clock position
   - Clockwise numbering
   - Displays hours (red)

3. **RUB3 (10 LEDs):** Linear bar below LCD
   - LED 0 on left
   - Displays air quality with color gradient

### Moon Phase Mechanism

- Stepper motor mounted below platform
- Shaft extends through platform center
- Decorative moon disk attached to shaft
- LDR sensor reads alignment hole in disk
- Calibration LED visible during setup

### Electronics Mounting

- Mount Arduino and driver boards under platform
- Use standoffs or mounting plate
- Keep wiring organized with cable ties
- Label all connections
- Provide ventilation for MQ135 sensor

## Hardware Testing

Before connecting all components, test each module individually using the test programs:

### Testing Sequence

1. **Basic Arduino Test**
   - File: `testing/test_codes/test_arduino/`
   - Verifies: Basic functionality, I2C bus, WiFi
   
2. **LED Test**
   - File: `testing/test_codes/test_led/`
   - Verifies: All three LED strips

3. **LCD Test**
   - File: `testing/test_codes/test_lcd/`
   - Verifies: Display and I2C address

4. **Button Test**
   - File: `testing/test_codes/test_button/`
   - Verifies: Button functionality and debouncing

5. **RTC Test**
   - File: `testing/test_codes/test_rtc/`
   - Verifies: DS3231 RTC and SQW interrupt

6. **Sensors Test**
   - File: `testing/test_codes/test_sensors/`
   - Verifies: DHT22 sensors and MQ135

7. **Moon Phase Test**
   - File: `testing/test_codes/test_moon_phase/`
   - Verifies: Stepper motor and calibration

See [Testing Guide](../testing/README.md) for detailed testing procedures.

## Troubleshooting

### General Issues

**Arduino not recognized by computer:**
- Install Arduino R4 WiFi drivers
- Try different USB cable (must be data cable, not charge-only)
- Try different USB port
- Check Device Manager (Windows) or lsusb (Linux)

**No power to components:**
- Verify power supply voltage (should be 5V ±5%)
- Check all ground connections
- Measure voltage at component VCC pins
- Check for loose connections

### I2C Issues

**I2C device not found:**
- Run I2C scanner test program
- Check SDA/SCL connections (A4/A5)
- Verify device has power
- Check for loose wires
- Verify pull-up resistors present

**I2C bus unstable/random errors:**
- Shorten I2C wires (keep under 30cm)
- Add external 4.7kΩ pull-up resistors
- Check for electrical noise sources
- Ensure common ground

### LED Issues

**LEDs not lighting:**
- Verify external 5V power supply connected
- Check ground connections (Arduino + LEDs + PSU)
- Test with simple LED test program
- Verify data pin connections (D9, D10, D11)
- Check LED strip polarity (5V, GND, DIN)

**LEDs flickering or wrong colors:**
- Insufficient power supply current
- Poor ground connections
- Data line too long (add 470Ω resistor at DIN)
- Add decoupling capacitor (1000µF) at power input

**First LED works, rest don't:**
- Broken data chain in LED strip
- Check DOUT to next DIN connection
- Test each section individually

### Sensor Issues

**DHT22 returns NaN (Not a Number):**
- Check 10kΩ pull-up resistor on data line
- Verify power supply (3.3V or 5V)
- Try longer delay between readings (3+ seconds)
- Replace sensor if consistently failing

**MQ135 gives constant readings:**
- Allow 24-48 hour burn-in period
- Check power connections
- Verify sensor is not faulty
- Test in different air conditions

**RTC time incorrect:**
- Set time using NTP sync or manual method
- Replace CR2032 battery on DS3231
- Check I2C connections
- Verify SQW pin connection for interrupt

### Stepper Motor Issues

**Motor not moving:**
- Check ULN2003 power connections
- Verify IN1-IN4 pin sequence
- Test motor with simple step sequence
- Check motor connector is fully seated

**Motor moves erratically:**
- Insufficient power supply
- Verify correct pin sequence in config.h
- Check for loose wire connections
- Test at slower speed

### WiFi Issues

**Cannot connect to WiFi:**
- Verify credentials in secrets.h
- Check WiFi is 2.4GHz (R4 doesn't support 5GHz)
- Move closer to router
- Check router MAC filtering
- Monitor serial output for error messages

**WiFi connects then disconnects:**
- Weak signal strength
- Power supply insufficient (WiFi uses more current)
- Router DHCP issues
- Interference from other devices

## Safety Notes

⚠️ **Important Safety Information:**

1. **Electrical Safety:**
   - Always disconnect power before making changes
   - Do not exceed component voltage ratings
   - Use appropriate wire gauges for current
   - Ensure proper insulation

2. **LED Safety:**
   - LEDs at full brightness can be very bright - avoid looking directly
   - High LED current can cause power supply overheating
   - Allow adequate ventilation

3. **Sensor Safety:**
   - MQ135 gets hot during operation (normal)
   - Keep flammable materials away from heated sensors
   - Ensure adequate ventilation

4. **General:**
   - Double-check all connections before applying power
   - Use appropriate fuses/protection
   - Keep liquids away from electronics
   - Follow local electrical codes and regulations

---

**Next Steps:**
- After completing hardware assembly, proceed to [Software Architecture](SOFTWARE.md)
- For component testing, see [Testing Guide](../testing/README.md)
- For configuration options, see [Configuration Guide](CONFIGURATION.md)