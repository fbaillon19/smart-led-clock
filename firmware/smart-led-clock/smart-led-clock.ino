/**
 * Smart LED Clock - Phase 3.1: Real Temperature Sensors
 * 
 * This version replaces simulated data with real DHT22 sensors:
 * - Indoor temperature and humidity
 * - Outdoor temperature and humidity
 * - Calculated values: feels-like, dew point, humidex
 * 
 * Components active in this phase:
 * - DS3231 RTC module
 * - 12 LED ring (hours)
 * - 60 LED ring (minutes/seconds)
 * - 20x4 I2C LCD display
 * - 2x DHT22 sensors (indoor/outdoor)
 * 
 * Author: F. Baillon
 * Version: Phase 3.1
 * Date: January 2025
 * License: GPL v3.0
 */

#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ==========================================
// PIN DEFINITIONS
// ==========================================
#define PIN_LEDS_MINUTE_SECOND  9
#define PIN_LEDS_HOUR          10
#define PIN_DHT_INDOOR          5
#define PIN_DHT_OUTDOOR         6

// ==========================================
// SENSOR CONFIGURATION
// ==========================================
#define DHT_TYPE                DHT22
#define SENSOR_READ_INTERVAL    2000  // Read sensors every 2 seconds

// ==========================================
// LED CONFIGURATION
// ==========================================
#define NUM_LEDS_HOUR           12
#define NUM_LEDS_MINUTE_SECOND  60

// LED Colors
#define COLOR_SECOND_R          0
#define COLOR_SECOND_G          127
#define COLOR_SECOND_B          0

#define COLOR_MINUTE_R          127
#define COLOR_MINUTE_G          0
#define COLOR_MINUTE_B          0

#define COLOR_OVERLAP_R         127
#define COLOR_OVERLAP_G         127
#define COLOR_OVERLAP_B         0

#define COLOR_HOUR_R            0
#define COLOR_HOUR_G            0
#define COLOR_HOUR_B            127

// ==========================================
// LCD CONFIGURATION
// ==========================================
#define LCD_I2C_ADDRESS         0x27
#define LCD_COLUMNS             20
#define LCD_ROWS                4

// ==========================================
// GLOBAL OBJECTS
// ==========================================
Adafruit_NeoPixel ledsHour(NUM_LEDS_HOUR, PIN_LEDS_HOUR, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledsMinuteSec(NUM_LEDS_MINUTE_SECOND, PIN_LEDS_MINUTE_SECOND, NEO_GRB + NEO_KHZ800);
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);
DHT dhtIndoor(PIN_DHT_INDOOR, DHT_TYPE);
DHT dhtOutdoor(PIN_DHT_OUTDOOR, DHT_TYPE);

// Custom degree symbol for LCD
byte degreeSymbol[8] = {
  0b01100,
  0b10010,
  0b10010,
  0b01100,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

// ==========================================
// SENSOR DATA STRUCTURE
// ==========================================
struct SensorData {
  float temperature;
  float humidity;
  float feelsLike;
  float dewPoint;
  int humidex;
  bool valid;
  unsigned long lastUpdate;
};

// ==========================================
// GLOBAL VARIABLES
// ==========================================
unsigned long lastSecondUpdate = 0;
unsigned long lastLCDUpdate = 0;
unsigned long lastSensorUpdate = 0;
unsigned short lastSecond = 61;
unsigned short lastMinute = 61;
unsigned short lastHour = 25;

bool isAnimationActive = false;
int animationStep = 0;
int animationHue = 0;

SensorData indoorData = {0, 0, 0, 0, 0, false, 0};
SensorData outdoorData = {0, 0, 0, 0, 0, false, 0};

// ==========================================
// SETUP
// ==========================================
void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== Smart LED Clock - Phase 3.1 ===");
  Serial.println("Initializing...");

  // Initialize I2C
  Wire.begin();
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, degreeSymbol);
  lcd.clear();
  
  displayStartupMessage("Initializing...");
  delay(1000);
  
  Serial.println("LCD initialized");

  // Initialize LED strips
  ledsHour.begin();
  ledsMinuteSec.begin();
  ledsHour.setBrightness(100);
  ledsMinuteSec.setBrightness(100);
  ledsHour.clear();
  ledsMinuteSec.clear();
  ledsHour.show();
  ledsMinuteSec.show();
  
  Serial.println("LED strips initialized");
  displayStartupMessage("LEDs Ready");
  delay(1000);

  // Initialize DHT22 sensors
  dhtIndoor.begin();
  dhtOutdoor.begin();
  Serial.println("DHT22 sensors initialized");
  displayStartupMessage("Sensors Ready");
  delay(1000);

  // Initialize DS3231 RTC
  if (!rtc.begin()) {
    Serial.println("ERROR: DS3231 not found!");
    displayStartupMessage("DS3231 ERROR!");
    while(1) {
      delay(1000);
    }
  }
  
  Serial.println("DS3231 RTC initialized");
  displayStartupMessage("DS3231 Ready");
  delay(1000);

  // Check if RTC lost power
  if (rtc.lostPower()) {
    Serial.println("WARNING: RTC lost power, setting time to compile time");
    displayStartupMessage("Setting time...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    delay(1000);
  }

  // Get current time
  DateTime now = rtc.now();
  Serial.print("Current time: ");
  printDateTime(now);
  Serial.println();

  // Initial sensor reading
  displayStartupMessage("Reading sensors...");
  updateSensorData();
  delay(2000);

  displayStartupMessage("System Ready!");
  delay(2000);
  
  lcd.clear();

  Serial.println("System ready!");
  Serial.println();
}

// ==========================================
// MAIN LOOP
// ==========================================
void loop() {
  static unsigned long lastAnimationUpdate = 0;

  // Update clock every second (when not animating)
  if (millis() - lastSecondUpdate >= 1000 && !isAnimationActive) {
    lastSecondUpdate = millis();
    
    DateTime now = rtc.now();
    updateLEDClock(now);
    
    // Check for hour change to trigger animation
    if (now.minute() == 0 && now.second() == 0 && !isAnimationActive) {
      startAnimation();
    }

    // Debug output every 10 seconds
    if (now.second() % 10 == 0) {
      Serial.print("Time: ");
      printDateTime(now);
      Serial.print(" | RTC: ");
      Serial.print(rtc.getTemperature());
      Serial.print("°C | Indoor: ");
      Serial.print(indoorData.temperature);
      Serial.print("°C | Outdoor: ");
      Serial.print(outdoorData.temperature);
      Serial.println("°C");
    }
  }

  // Update sensors every 2 seconds
  if (millis() - lastSensorUpdate >= SENSOR_READ_INTERVAL) {
    lastSensorUpdate = millis();
    updateSensorData();
  }

  // Update LCD display every 2 seconds (when not animating)
  if (millis() - lastLCDUpdate >= 2000 && !isAnimationActive) {
    lastLCDUpdate = millis();
    DateTime now = rtc.now();
    updateLCDDisplay(now);
  }

  // Handle animation
  if (isAnimationActive) {
    if (millis() - lastAnimationUpdate >= 50) {
      if (!updateAnimation()) {
        stopAnimation();
      }
      lastAnimationUpdate = millis();
    }
  }

  delay(10);
}

// ==========================================
// SENSOR FUNCTIONS
// ==========================================
void updateSensorData() {
  // Read indoor sensor
  float tempIn = dhtIndoor.readTemperature();
  float humIn = dhtIndoor.readHumidity();
  
  if (!isnan(tempIn) && !isnan(humIn)) {
    indoorData.temperature = tempIn;
    indoorData.humidity = humIn;
    indoorData.feelsLike = dhtIndoor.computeHeatIndex(tempIn, humIn, false);
    indoorData.dewPoint = calculateDewPoint(tempIn, humIn);
    indoorData.humidex = calculateHumidex(tempIn, humIn);
    indoorData.valid = true;
    indoorData.lastUpdate = millis();
  } else {
    indoorData.valid = false;
    Serial.println("ERROR: Indoor sensor read failed");
  }

  // Read outdoor sensor
  float tempOut = dhtOutdoor.readTemperature();
  float humOut = dhtOutdoor.readHumidity();
  
  if (!isnan(tempOut) && !isnan(humOut)) {
    outdoorData.temperature = tempOut;
    outdoorData.humidity = humOut;
    outdoorData.feelsLike = dhtOutdoor.computeHeatIndex(tempOut, humOut, false);
    outdoorData.dewPoint = calculateDewPoint(tempOut, humOut);
    outdoorData.humidex = calculateHumidex(tempOut, humOut);
    outdoorData.valid = true;
    outdoorData.lastUpdate = millis();
  } else {
    outdoorData.valid = false;
    Serial.println("ERROR: Outdoor sensor read failed");
  }
}

float calculateDewPoint(float temp, float humidity) {
  // Magnus-Tetens formula
  float alpha = log(humidity / 100.0) + (17.27 * temp) / (237.3 + temp);
  return (237.3 * alpha) / (17.27 - alpha);
}

int calculateHumidex(float temp, float humidity) {
  // Calculate dew point first
  float dewPoint = calculateDewPoint(temp, humidity);
  
  // Masterton and Richardson equation
  float e = 6.11 * exp(5417.753 * (1.0/273.16 - 1.0/(273.15 + dewPoint)));
  float humidex = temp + 0.5555 * (e - 10.0);
  
  return (int)humidex;
}

const char* getHumidexDescription(int humidex) {
  if (humidex < 20) return "Pas d'inconfort";
  if (humidex < 30) return "Peu d'inconfort";
  if (humidex < 40) return "Inconfort certain";
  if (humidex < 45) return "Eviter efforts";
  return "Danger chaleur";
}

// ==========================================
// LCD DISPLAY FUNCTIONS
// ==========================================
void updateLCDDisplay(DateTime now) {
  // Line 0: Date
  lcd.setCursor(0, 0);
  char dateBuffer[21];
  sprintf(dateBuffer, "%02d/%02d/%04d", now.day(), now.month(), now.year());
  lcd.print(dateBuffer);
  
  const char* daysOfWeek[] = {"Dim", "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam"};
  lcd.setCursor(12, 0);
  lcd.print(daysOfWeek[now.dayOfTheWeek()]);
  lcd.print("    ");

  // Line 1: Time
  lcd.setCursor(0, 1);
  char timeBuffer[21];
  sprintf(timeBuffer, "Heure: %02d:%02d:%02d", now.hour(), now.minute(), now.second());
  lcd.print(timeBuffer);
  lcd.print("    ");

  // Line 2: Indoor sensor
  lcd.setCursor(0, 2);
  if (indoorData.valid) {
    lcd.print("Int: ");
    lcd.print(indoorData.temperature, 1);
    lcd.write(byte(0));
    lcd.print("C ");
    lcd.print((int)indoorData.humidity);
    lcd.print("%");
    lcd.print("   ");
  } else {
    lcd.print("Int: ERREUR        ");
  }

  // Line 3: Outdoor sensor
  lcd.setCursor(0, 3);
  if (outdoorData.valid) {
    lcd.print("Ext: ");
    lcd.print(outdoorData.temperature, 1);
    lcd.write(byte(0));
    lcd.print("C ");
    lcd.print((int)outdoorData.humidity);
    lcd.print("%");
    lcd.print("   ");
  } else {
    lcd.print("Ext: ERREUR        ");
  }
}

void displayStartupMessage(const char* message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart LED Clock");
  lcd.setCursor(0, 1);
  lcd.print("Phase 3.1");
  lcd.setCursor(0, 3);
  lcd.print(message);
}

// ==========================================
// LED CLOCK DISPLAY
// ==========================================
void updateLEDClock(DateTime now) {
  int hour = now.hour() % 12;
  int minute = now.minute();
  int second = now.second();

  // Update hour if changed
  if (hour != lastHour) {
    ledsHour.setPixelColor(lastHour, 0, 0, 0);
    lastHour = hour;
  }
  ledsHour.setPixelColor(hour, COLOR_HOUR_R, COLOR_HOUR_G, COLOR_HOUR_B);
  ledsHour.show();

  // Update minute if changed
  if (minute != lastMinute) {
    ledsMinuteSec.setPixelColor(lastMinute, 0, 0, 0);
    lastMinute = minute;
  }

  // Clear previous second
  if (lastSecond != 61 && lastSecond != minute) {
    ledsMinuteSec.setPixelColor(lastSecond, 0, 0, 0);
  }

  // Display minute
  if (minute == second) {
    ledsMinuteSec.setPixelColor(minute, COLOR_OVERLAP_R, COLOR_OVERLAP_G, COLOR_OVERLAP_B);
  } else {
    ledsMinuteSec.setPixelColor(minute, COLOR_MINUTE_R, COLOR_MINUTE_G, COLOR_MINUTE_B);
    ledsMinuteSec.setPixelColor(second, COLOR_SECOND_R, COLOR_SECOND_G, COLOR_SECOND_B);
  }

  ledsMinuteSec.show();
  lastSecond = second;
}

// ==========================================
// ANIMATION FUNCTIONS
// ==========================================
void startAnimation() {
  Serial.println("Starting hourly animation");
  
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("  Animation horaire");
  
  isAnimationActive = true;
  animationStep = 0;
  animationHue = 0;
  ledsMinuteSec.clear();
  ledsMinuteSec.show();
}

bool updateAnimation() {
  if (animationStep >= 100) {
    return false;
  }

  ledsMinuteSec.clear();

  for (int i = animationStep % 3; i < NUM_LEDS_MINUTE_SECOND; i += 3) {
    int hue = animationHue + (i * 65536L / NUM_LEDS_MINUTE_SECOND);
    uint32_t color = ledsMinuteSec.gamma32(ledsMinuteSec.ColorHSV(hue));
    ledsMinuteSec.setPixelColor(i, color);
  }

  ledsMinuteSec.show();
  animationHue += 65536 / 100;
  animationStep++;
  
  return true;
}

void stopAnimation() {
  Serial.println("Animation complete");
  isAnimationActive = false;
  ledsMinuteSec.clear();
  ledsMinuteSec.show();
  
  lcd.clear();
  
  DateTime now = rtc.now();
  lastSecond = 61;
  lastMinute = 61;
  lastHour = 25;
  updateLEDClock(now);
  updateLCDDisplay(now);
}

// ==========================================
// UTILITY FUNCTIONS
// ==========================================
void printDateTime(DateTime dt) {
  char buffer[20];
  sprintf(buffer, "%04d/%02d/%02d %02d:%02d:%02d", 
          dt.year(), dt.month(), dt.day(),
          dt.hour(), dt.minute(), dt.second());
  Serial.print(buffer);
}