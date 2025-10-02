# Smart LED Clock - Installation Guide

This guide will walk you through the complete installation process, from gathering components to having your Smart LED Clock up and running.

## 📋 Prerequisites

### Skills Required
- **Basic electronics knowledge** (reading schematics, using breadboards)
- **Soldering experience** (connecting components, making reliable joints)
- **Arduino IDE familiarity** (uploading sketches, installing libraries)
- **Basic WiFi setup** (connecting devices to your network)

### Tools Needed
- Soldering iron (temperature controlled recommended)
- Solder (60/40 or 63/37 rosin core)
- Wire strippers
- Multimeter
- Breadboard or perfboard for prototyping
- Hot glue gun (for final assembly)
- Computer with Arduino IDE installed

## 🛒 Step 1: Gather Components

### Order Components
See [HARDWARE.md](../HARDWARE.md) for the complete components list with estimated prices (~€136 total).

**Critical components to verify:**
- ✅ Arduino UNO R4 **WiFi** (not the standard R4)
- ✅ WS2812B LED strips (not WS2811 or other types)
- ✅ DHT22 sensors (DHT11 won't work as well)
- ✅ 20x4 LCD with I2C backpack (not parallel connection)

### Organize Your Workspace
- Clean, well-lit work area
- Anti-static mat (recommended)
- Good ventilation for soldering
- Component storage boxes

## 🔧 Step 2: Software Setup

### Install Arduino IDE
1. Download Arduino IDE 2.x from [arduino.cc](https://www.arduino.cc/en/software)
2. Install for your operating system
3. Launch and verify it works

### Install Required Libraries
In Arduino IDE, go to **Tools > Manage Libraries** and install:

```
Search and Install:
├── "Adafruit NeoPixel" by Adafruit
├── "DHT sensor library" by Adafruit  
├── "LiquidCrystal I2C" by Frank de Brabander
├── "OneButton" by Matthias Hertel
└── "NTPClient" by Fabrice Weinberg
```

**Note:** WiFiS3 and RTC libraries are built into the Arduino UNO R4 core.

### Download Project Code
1. Clone or download this repository
2. Extract to your Arduino sketches folder
3. Rename the main folder to `smart-led-clock` if needed

## ⚡ Step 3: Basic Circuit Assembly

### Safety First
- 🔌 **Always disconnect power** when making connections
- 🔍 **Double-check wiring** before applying power
- ⚡ **Use 5V supply only** - higher voltage will damage components

### Start with Power Distribution
1. **Create power buses** on your breadboard or perfboard
2. **Connect 5V rail** to Arduino 5V pin
3. **Connect GND rail** to Arduino GND pin
4. **Test voltage** with multimeter (should read ~5V)

### Wire Components Step by Step

#### Phase 1: LCD Display (Easiest First)
```
LCD I2C → Arduino UNO R4
├── VCC → 5V
├── GND → GND
├── SDA → A4 (SDA)
└── SCL → A5 (SCL)
```

**Test LCD:**
1. Upload a simple I2C scanner sketch to find LCD address
2. Verify LCD lights up and displays text
3. Default address is usually 0x27

#### Phase 2: Temperature Sensors
```
DHT22 Indoor → Arduino
├── VCC → 5V
├── GND → GND
└── DATA → D6

DHT22 Outdoor → Arduino  
├── VCC → 5V
├── GND → GND
└── DATA → D7
```

**Test sensors:**
1. Upload DHT22 test sketch
2. Verify both sensors report reasonable temperatures
3. Indoor should be ~20-25°C, outdoor varies

#### Phase 3: Push Buttons
```
Button 1 → Arduino
├── One leg → D2
└── Other leg → GND
(Use internal pull-up: INPUT_PULLUP)

Button 2 → Arduino
├── One leg → D3  
└── Other leg → GND
```

#### Phase 4: Air Quality Sensor
```
MQ135 → Arduino
├── VCC → 5V
├── GND → GND
└── AOUT → A0
```

**Important:** MQ135 needs 24-48h "burn-in" time for stable readings.

#### Phase 5: LED Strips (Most Complex)
```
LED Strip 60 (Minutes/Seconds) → Arduino
├── 5V (Red) → 5V rail
├── GND (Black) → GND rail
└── DIN (White/Green) → D9

LED Strip 12 (Hours) → Arduino
├── 5V (Red) → 5V rail  
├── GND (Black) → GND rail
└── DIN (White/Green) → D10

LED Strip 10 (Air Quality) → Arduino
├── 5V (Red) → 5V rail
├── GND (Black) → GND rail
└── DIN (White/Green) → D11
```

**LED Strip Tips:**
- ⚡ **Power consumption**: 60mA per LED at full white brightness
- 🔗 **Chaining**: Connect DOUT of one strip to DIN of next if chaining
- 🔧 **Connections**: Solder connections for reliability
- 💡 **Test individually**: Upload LED test code for each strip

## 💻 Step 4: Software Configuration

### Setup WiFi Credentials
1. Copy `secrets.template.h` to `secrets.h`
2. Edit `secrets.h` with your WiFi details:
```cpp
const char* ssid = "YourWiFiName";
const char* pass = "YourWiFiPassword";
```

### Configure Hardware Settings
Edit `config.h` if you need to change:
- Pin assignments
- LED counts  
- I2C addresses
- Timing settings

**Most users can keep default settings.**

### Upload the Code
1. Connect Arduino UNO R4 WiFi via USB
2. Select **Board**: "Arduino UNO R4 WiFi"
3. Select correct **Port**
4. Click **Upload** (➤ button)

### First Boot Sequence
Your clock will display:
```
Smart LED Clock v1.0
→ Sensors initialized
→ Connecting WiFi...
→ Time synchronized  
→ Starting web server...
→ Web server ready
→ System Ready!
```

## 🌐 Step 5: Web Interface Setup

### Find Your Clock's IP Address
The IP address will be shown on the LCD during startup, or check your router's connected devices.

### Access Web Interface
1. Open web browser
2. Navigate to `http://[IP_ADDRESS]`
3. You should see the Smart LED Clock dashboard

### Initial Configuration
**Recommended settings for first use:**
- LED Brightness: 50% (to avoid being too bright)
- Night Mode: Enabled (22:00 - 07:00)
- Animations: Enabled
- Sensor Update: 2 seconds

## 🧪 Step 6: Testing and Calibration

### System Tests Checklist

#### ✅ Visual Tests
- [ ] All LED rings light up
- [ ] LCD shows temperature data
- [ ] Button presses change display
- [ ] Air quality bar responds to breath/air freshener

#### ✅ Time Functions
- [ ] Clock displays correct time
- [ ] Second hand moves smoothly
- [ ] Minute hand advances correctly
- [ ] Hour animations play (wait for next hour)

#### ✅ Sensor Validation
- [ ] Indoor temperature reasonable (20-25°C typically)
- [ ] Outdoor sensor different from indoor
- [ ] Humidity readings 30-70% typically
- [ ] Air quality changes with environment

#### ✅ Network Functions
- [ ] WiFi connects automatically
- [ ] Web interface accessible
- [ ] Settings changes apply immediately
- [ ] Time syncs with internet

### Troubleshooting Common Issues

**LEDs not working:**
```bash
Symptom: No LED activity
Causes: 
├── Power supply insufficient (need 5V 3A+)
├── Wrong pin connections
├── Bad solder joints
└── Incorrect LED strip type
```

**Temperature shows NaN:**
```bash
Symptom: "NaN" on display
Causes:
├── DHT22 wiring incorrect  
├── Sensor damaged
├── Power supply unstable
└── Wrong pin assignment
```

**WiFi won't connect:**
```bash
Symptom: "WiFi connection failed"
Causes:
├── Wrong credentials in secrets.h
├── WiFi signal too weak
├── Network uses unsupported security
└── Arduino WiFi module issue
```

**Web interface not accessible:**
```bash
Symptom: Browser can't reach clock
Causes:
├── Clock not connected to WiFi
├── Firewall blocking connection
├── Wrong IP address
└── Web server crashed
```

## 🎨 Step 7: Final Assembly

### Mechanical Assembly Tips
1. **Plan your layout** - sketch the 30cm circular arrangement first
2. **Mount components securely** - use standoffs for Arduino, hot glue for sensors
3. **Route wires neatly** - keep power and data wires separated
4. **Strain relief** - secure wires at connection points
5. **Access panels** - ensure you can reach buttons and USB port

### Enclosure Options
- **3D printed case** - custom design for perfect fit
- **Acrylic circle** - laser-cut clear or colored acrylic
- **Wood base** - natural material for living room
- **Metal plate** - industrial look with proper grounding

### Weatherproofing (for outdoor sensor)
- IP65 enclosure for outdoor DHT22
- Weatherproof cable glands
- Silicone sealant for connections
- Shade from direct sunlight

## 🚀 Step 8: Going Live

### Final Configuration
1. **Set brightness** appropriate for room
2. **Enable night mode** for bedroom use
3. **Test all display modes** with buttons
4. **Verify animations** work at next hour change
5. **Check web interface** from phone/tablet

### Daily Operation
- Clock automatically syncs time daily at 1:01 AM
- Sensors update every 2 seconds
- Web interface shows real-time data
- Buttons cycle through display modes

### Backup and Updates
- **Save your secrets.h** file securely
- **Document any custom changes** you made
- **Check GitHub** for updates periodically
- **Consider data logging** via web interface

## ✅ Success Checklist

**Your Smart LED Clock is complete when:**
- [ ] Time displays accurately with LED rings
- [ ] Temperature sensors show reasonable values
- [ ] Air quality bar responds to environment changes
- [ ] Web interface accessible and responsive
- [ ] Animations play beautifully every hour
- [ ] Night mode dims display appropriately
- [ ] All buttons function correctly
- [ ] System runs stable for 24+ hours

## 🎉 Congratulations!

You've successfully built a Smart LED Clock! Share photos of your build and consider contributing improvements back to the project.

## 📞 Support

**If you encounter issues:**

1. **Check the troubleshooting section** above
2. **Review your wiring** against HARDWARE.md
3. **Test components individually** before integration
4. **Open an issue** on GitHub with photos and error descriptions
5. **Join community discussions** for tips and modifications

**Remember:** This is a learning project - debugging and improvement are part of the fun!

---

**Next Steps:**
- Explore the web interface features
- Try different LED color schemes
- Add weather API integration (future feature)
- Design a custom enclosure
- Share your build with the community!