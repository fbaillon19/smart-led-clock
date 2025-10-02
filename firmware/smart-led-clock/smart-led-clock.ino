/**
 * Smart LED Clock - Phase 4: WiFi + NTP Synchronization
 * 
 * This version adds network capabilities:
 * - WiFi connection at startup
 * - NTP time synchronization with DS3231
 * - Daily automatic sync at 1:01 AM
 * - WiFi status display on LCD
 * - Network error handling
 * 
 * Components active in this phase:
 * - DS3231 RTC module
 * - 12 LED ring (hours)
 * - 60 LED ring (minutes/seconds)
 * - 10 LED bar (air quality)
 * - 20x4 I2C LCD display
 * - 2x DHT22 sensors
 * - MQ135 air quality sensor
 * - WiFi (Arduino UNO R4 WiFi)
 * 
 * Author: F. Baillon
 * Version: Phase 4
 * Date: January 2025
 * License: GPL v3.0
 */

#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// WiFi credentials - create secrets.h with:
// const char* ssid = "YourSSID";
// const char* pass = "YourPassword";
#include "secrets.h"

// ==========================================
// PIN DEFINITIONS
// ==========================================
#define PIN_LEDS_MINUTE_SECOND  9
#define PIN_LEDS_HOUR          10
#define PIN_LEDS_AIR_QUALITY   11
#define PIN_DHT_INDOOR          5
#define PIN_DHT_OUTDOOR         6
#define PIN_AIR_QUALITY_SENSOR  A0

// ==========================================
// CONFIGURATION
// ==========================================
#define DHT_TYPE                DHT22
#define SENSOR_READ_INTERVAL    2000
#define NTP_SYNC_HOUR           1     // Daily sync at 1 AM
#define NTP_SYNC_MINUTE         1
#define TIME_ZONE_OFFSET        2     // UTC+2 for Paris (summer time CEST)

// ==========================================
// LED CONFIGURATION
// ==========================================
#define NUM_LEDS_HOUR           12
#define NUM_LEDS_MINUTE_SECOND  60
#define NUM_LEDS_AIR_QUALITY    10

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
Adafruit_NeoPixel ledsAirQuality(NUM_LEDS_AIR_QUALITY, PIN_LEDS_AIR_QUALITY, NEO_GRB + NEO_KHZ800);
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);
DHT dhtIndoor(PIN_DHT_INDOOR, DHT_TYPE);
DHT dhtOutdoor(PIN_DHT_OUTDOOR, DHT_TYPE);

WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", TIME_ZONE_OFFSET * 3600, 60000);

byte degreeSymbol[8] = {
  0b01100, 0b10010, 0b10010, 0b01100,
  0b00000, 0b00000, 0b00000, 0b00000
};

// ==========================================
// SENSOR DATA STRUCTURES
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

struct AirQualityData {
  int rawADC;
  int estimatedAQI;
  const char* quality;
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
AirQualityData airQuality = {0, 0, "Unknown", false, 0};

int lastAirQualityValue = -1;
bool wifiConnected = false;
bool lastNTPSyncSuccess = false;
unsigned long lastNTPSync = 0;

// ==========================================
// SETUP
// ==========================================
void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== Smart LED Clock - Phase 4 ===");
  Serial.println("Initializing...");

  Wire.begin();
  
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, degreeSymbol);
  lcd.clear();
  
  displayStartupMessage("Initializing...");
  delay(1000);

  ledsHour.begin();
  ledsMinuteSec.begin();
  ledsAirQuality.begin();
  ledsHour.setBrightness(100);
  ledsMinuteSec.setBrightness(100);
  ledsAirQuality.setBrightness(100);
  ledsHour.clear();
  ledsMinuteSec.clear();
  ledsAirQuality.clear();
  ledsHour.show();
  ledsMinuteSec.show();
  ledsAirQuality.show();
  
  Serial.println("LED strips initialized");
  displayStartupMessage("LEDs Ready");
  delay(1000);

  pinMode(PIN_AIR_QUALITY_SENSOR, INPUT);
  Serial.println("MQ135 initialized");
  
  dhtIndoor.begin();
  dhtOutdoor.begin();
  Serial.println("DHT22 sensors initialized");
  displayStartupMessage("Sensors Ready");
  delay(1000);

  if (!rtc.begin()) {
    Serial.println("ERROR: DS3231 not found!");
    displayStartupMessage("DS3231 ERROR!");
    while(1) delay(1000);
  }
  
  Serial.println("DS3231 RTC initialized");
  displayStartupMessage("DS3231 Ready");
  delay(1000);

  // Connect WiFi and sync with NTP
  displayStartupMessage("Connecting WiFi...");
  if (connectWiFi()) {
    wifiConnected = true;
    Serial.println("WiFi connected");
    displayStartupMessage("WiFi Connected");
    delay(1000);
    
    displayStartupMessage("Syncing time...");
    if (syncTimeWithNTP()) {
      lastNTPSyncSuccess = true;
      Serial.println("NTP sync successful");
      displayStartupMessage("Time Synchronized");
    } else {
      Serial.println("NTP sync failed, using RTC time");
      displayStartupMessage("Using RTC time");
    }
  } else {
    Serial.println("WiFi connection failed, using RTC time");
    displayStartupMessage("No WiFi - RTC time");
  }
  delay(2000);

  DateTime now = rtc.now();
  Serial.print("Current time: ");
  printDateTime(now);
  Serial.println();

  updateSensorData();
  updateAirQuality();

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

  if (millis() - lastSecondUpdate >= 1000 && !isAnimationActive) {
    lastSecondUpdate = millis();
    
    DateTime now = rtc.now();
    updateLEDClock(now);
    
    if (now.minute() == 0 && now.second() == 0 && !isAnimationActive) {
      startAnimation();
    }

    // Check for daily NTP sync at 1:01 AM
    if (wifiConnected && now.hour() == NTP_SYNC_HOUR && 
        now.minute() == NTP_SYNC_MINUTE && now.second() == 0) {
      Serial.println("Daily NTP sync triggered");
      syncTimeWithNTP();
    }

    if (now.second() % 10 == 0) {
      Serial.print("Time: ");
      printDateTime(now);
      Serial.print(" | RTC: ");
      Serial.print(rtc.getTemperature());
      Serial.print("°C | Indoor: ");
      Serial.print(indoorData.temperature);
      Serial.print("°C | AQI: ");
      Serial.println(airQuality.estimatedAQI);
    }
  }

  if (millis() - lastSensorUpdate >= SENSOR_READ_INTERVAL) {
    lastSensorUpdate = millis();
    updateSensorData();
    updateAirQuality();
  }

  if (millis() - lastLCDUpdate >= 2000 && !isAnimationActive) {
    lastLCDUpdate = millis();
    DateTime now = rtc.now();
    updateLCDDisplay(now);
  }

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
// WIFI & NTP FUNCTIONS
// ==========================================
bool connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  int attempts = 0;
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    // Wait a bit for DHCP to assign IP
    delay(1000);
    
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());
    return true;
  }
  
  Serial.println("WiFi connection failed");
  return false;
}

bool syncTimeWithNTP() {
  Serial.println("Synchronizing with NTP server...");
  
  timeClient.begin();
  
  int attempts = 0;
  while (!timeClient.update() && attempts < 10) {
    timeClient.forceUpdate();
    delay(500);
    attempts++;
  }
  
  if (timeClient.isTimeSet()) {
    unsigned long epochTime = timeClient.getEpochTime();
    rtc.adjust(DateTime(epochTime));
    
    lastNTPSync = millis();
    
    Serial.print("Time synchronized: ");
    DateTime now = rtc.now();
    printDateTime(now);
    Serial.println();
    
    return true;
  }
  
  Serial.println("NTP sync failed");
  return false;
}

// ==========================================
// SENSOR FUNCTIONS (unchanged from Phase 3.2)
// ==========================================
void updateSensorData() {
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
  }

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
  }
}

float calculateDewPoint(float temp, float humidity) {
  float alpha = log(humidity / 100.0) + (17.27 * temp) / (237.3 + temp);
  return (237.3 * alpha) / (17.27 - alpha);
}

int calculateHumidex(float temp, float humidity) {
  float dewPoint = calculateDewPoint(temp, humidity);
  float e = 6.11 * exp(5417.753 * (1.0/273.16 - 1.0/(273.15 + dewPoint)));
  return (int)(temp + 0.5555 * (e - 10.0));
}

void updateAirQuality() {
  airQuality.rawADC = analogRead(PIN_AIR_QUALITY_SENSOR);
  airQuality.estimatedAQI = constrain(airQuality.rawADC / 5, 0, 500);
  
  if (airQuality.estimatedAQI <= 50) airQuality.quality = "Good";
  else if (airQuality.estimatedAQI <= 100) airQuality.quality = "Moderate";
  else if (airQuality.estimatedAQI <= 150) airQuality.quality = "Unhealthy-S";
  else if (airQuality.estimatedAQI <= 200) airQuality.quality = "Unhealthy";
  else if (airQuality.estimatedAQI <= 300) airQuality.quality = "Very Poor";
  else airQuality.quality = "Hazardous";
  
  airQuality.valid = true;
  airQuality.lastUpdate = millis();
  
  if (abs(airQuality.rawADC - lastAirQualityValue) > 5) {
    updateAirQualityLEDs();
    lastAirQualityValue = airQuality.rawADC;
  }
}

void updateAirQualityLEDs() {
  int brightness = constrain(20 + (airQuality.estimatedAQI / 10), 20, 60);
  ledsAirQuality.setBrightness(brightness);
  ledsAirQuality.clear();
  
  int baseHue;
  if (airQuality.estimatedAQI <= 50) {
    baseHue = map(airQuality.estimatedAQI, 0, 50, 26000, 21845);
  } else if (airQuality.estimatedAQI <= 100) {
    baseHue = map(airQuality.estimatedAQI, 50, 100, 21845, 16384);
  } else if (airQuality.estimatedAQI <= 200) {
    baseHue = map(airQuality.estimatedAQI, 100, 200, 16384, 4096);
  } else {
    baseHue = map(airQuality.estimatedAQI, 200, 500, 4096, 0);
  }
  
  for (int i = 0; i < NUM_LEDS_AIR_QUALITY; i++) {
    int hueOffset = (i - NUM_LEDS_AIR_QUALITY / 2) * 500;
    int ledHue = baseHue + hueOffset;
    if (ledHue < 0) ledHue += 65536;
    if (ledHue >= 65536) ledHue -= 65536;
    
    uint32_t color = ledsAirQuality.gamma32(ledsAirQuality.ColorHSV(ledHue));
    ledsAirQuality.setPixelColor(i, color);
  }
  
  ledsAirQuality.show();
}

// ==========================================
// LCD DISPLAY
// ==========================================
void updateLCDDisplay(DateTime now) {
  lcd.setCursor(0, 0);
  char dateBuffer[21];
  sprintf(dateBuffer, "%02d/%02d/%04d", now.day(), now.month(), now.year());
  lcd.print(dateBuffer);
  
  const char* daysOfWeek[] = {"Dim", "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam"};
  lcd.setCursor(12, 0);
  lcd.print(daysOfWeek[now.dayOfTheWeek()]);
  lcd.print("    ");

  lcd.setCursor(0, 1);
  char timeBuffer[21];
  sprintf(timeBuffer, "Heure: %02d:%02d:%02d", now.hour(), now.minute(), now.second());
  lcd.print(timeBuffer);
  lcd.print("    ");

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

  lcd.setCursor(0, 3);
  if (outdoorData.valid) {
    lcd.print("Ext:");
    lcd.print(outdoorData.temperature, 1);
    lcd.write(byte(0));
  } else {
    lcd.print("Ext:ERR ");
  }
  
  lcd.print(" AQI:");
  lcd.print(airQuality.estimatedAQI);
  lcd.print("    ");
}

void displayStartupMessage(const char* message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart LED Clock");
  lcd.setCursor(0, 1);
  lcd.print("Phase 4");
  lcd.setCursor(0, 3);
  lcd.print(message);
}

// ==========================================
// LED CLOCK & ANIMATION (unchanged)
// ==========================================
void updateLEDClock(DateTime now) {
  int hour = now.hour() % 12;
  int minute = now.minute();
  int second = now.second();

  if (hour != lastHour) {
    ledsHour.setPixelColor(lastHour, 0, 0, 0);
    lastHour = hour;
  }
  ledsHour.setPixelColor(hour, COLOR_HOUR_R, COLOR_HOUR_G, COLOR_HOUR_B);
  ledsHour.show();

  if (minute != lastMinute) {
    ledsMinuteSec.setPixelColor(lastMinute, 0, 0, 0);
    lastMinute = minute;
  }

  if (lastSecond != 61 && lastSecond != minute) {
    ledsMinuteSec.setPixelColor(lastSecond, 0, 0, 0);
  }

  if (minute == second) {
    ledsMinuteSec.setPixelColor(minute, COLOR_OVERLAP_R, COLOR_OVERLAP_G, COLOR_OVERLAP_B);
  } else {
    ledsMinuteSec.setPixelColor(minute, COLOR_MINUTE_R, COLOR_MINUTE_G, COLOR_MINUTE_B);
    ledsMinuteSec.setPixelColor(second, COLOR_SECOND_R, COLOR_SECOND_G, COLOR_SECOND_B);
  }

  ledsMinuteSec.show();
  lastSecond = second;
}

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
  if (animationStep >= 100) return false;

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

void printDateTime(DateTime dt) {
  char buffer[20];
  sprintf(buffer, "%04d/%02d/%02d %02d:%02d:%02d", 
          dt.year(), dt.month(), dt.day(),
          dt.hour(), dt.minute(), dt.second());
  Serial.print(buffer);
}