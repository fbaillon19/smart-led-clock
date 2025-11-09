/**
 * Smart LED Clock - Sensors Test
 * 
 * This test verifies that all sensors are working correctly:
 * - DHT22 Indoor temperature/humidity sensor
 * - DHT22 Outdoor temperature/humidity sensor  
 * - MQ135 Air quality sensor
 * 
 * Expected Results:
 * - Both DHT22 sensors should report reasonable temperature/humidity values
 * - MQ135 should provide analog readings that change with air conditions
 * - Serial monitor shows continuous sensor readings
 * - Calculated values (heat index, dew point, humidex) should be reasonable
 * 
 * Author: F. Baillon
 * License: MIT
 */

#include <DHT.h>

// Pin definitions (match your actual wiring)
#define PIN_DHT_INDOOR     5    // Indoor DHT22 sensor
#define PIN_DHT_OUTDOOR    6    // Outdoor DHT22 sensor  
#define PIN_AIR_QUALITY    A0   // MQ135 air quality sensor

// DHT sensor type
#define DHT_TYPE DHT22

// Create DHT sensor objects
DHT dhtIndoor(PIN_DHT_INDOOR, DHT_TYPE);
DHT dhtOutdoor(PIN_DHT_OUTDOOR, DHT_TYPE);

// Test timing
unsigned long lastReading = 0;
const unsigned long READING_INTERVAL = 3000; // Read every 3 seconds
int readingCount = 0;

// Sensor validation ranges (reasonable values)
#define MIN_TEMP -40.0
#define MAX_TEMP 80.0
#define MIN_HUMIDITY 0.0
#define MAX_HUMIDITY 100.0
#define MIN_AIR_QUALITY 0
#define MAX_AIR_QUALITY 1023

// Error tracking
int indoorErrors = 0;
int outdoorErrors = 0;
int airQualityErrors = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=====================================");
  Serial.println("    Smart LED Clock - Sensors Test");
  Serial.println("=====================================");
  Serial.println();
  
  // Initialize sensors
  Serial.println("Initializing sensors...");
  
  // Initialize DHT sensors
  dhtIndoor.begin();
  dhtOutdoor.begin();
  
  // Initialize air quality sensor pin
  pinMode(PIN_AIR_QUALITY, INPUT);
  
  Serial.println("DHT22 Indoor sensor initialized on pin " + String(PIN_DHT_INDOOR));
  Serial.println("DHT22 Outdoor sensor initialized on pin " + String(PIN_DHT_OUTDOOR));
  Serial.println("MQ135 Air quality sensor initialized on pin A" + String(PIN_AIR_QUALITY - A0));
  Serial.println();
  
  Serial.println("Waiting 2 seconds for sensors to stabilize...");
  delay(2000);
  
  Serial.println("Starting continuous sensor readings...");
  Serial.println("(Breathe on sensors or use hair dryer to see changes)");
  Serial.println();
  
  // Print header for data table
  printDataHeader();
}

void loop() {
  // Read sensors at specified interval
  if (millis() - lastReading >= READING_INTERVAL) {
    readingCount++;
    
    Serial.print("Reading #");
    Serial.print(readingCount);
    Serial.print(" - ");
    Serial.print("Time: ");
    Serial.print(millis() / 1000);
    Serial.println("s");
    
    // Test all sensors
    testIndoorSensor();
    testOutdoorSensor();
    testAirQualitySensor();
    
    // Print summary line
    printSummary();
    Serial.println();
    
    lastReading = millis();
  }
  
  // Print status every 30 seconds
  static unsigned long lastStatus = 0;
  if (millis() - lastStatus >= 30000) {
    printErrorSummary();
    lastStatus = millis();
  }
  
  delay(100);
}

/**
 * Print header for sensor data table
 */
void printDataHeader() {
  Serial.println("┌──────────────────────────────────────────────────────────────────────┐");
  Serial.println("│                           SENSOR READINGS                               │");
  Serial.println("├──────────────────────────────────────────────────────────────────────┤");
  Serial.println("│ Sensor    │ Temp(°C) │ Hum(%) │ Heat Idx │ Dew Pt │ Humidx │ Status │");
  Serial.println("├──────────────────────────────────────────────────────────────────────┤");
}

/**
 * Test indoor DHT22 sensor
 */
void testIndoorSensor() {
  float humidity = dhtIndoor.readHumidity();
  float temperature = dhtIndoor.readTemperature();
  
  Serial.print("│ Indoor    │ ");
  
  if (isnan(humidity) || isnan(temperature)) {
    Serial.print("   ERROR   │  ERROR │   ERROR  │  ERROR │  ERROR │  FAIL  │");
    indoorErrors++;
    return;
  }
  
  // Validate readings
  bool tempValid = (temperature >= MIN_TEMP && temperature <= MAX_TEMP);
  bool humValid = (humidity >= MIN_HUMIDITY && humidity <= MAX_HUMIDITY);
  
  if (!tempValid || !humValid) {
    Serial.print("INVALID   │ INVALID│  INVALID │ INVALID│ INVALID│  WARN  │");
    indoorErrors++;
    return;
  }
  
  // Calculate derived values
  float heatIndex = dhtIndoor.computeHeatIndex(temperature, humidity, false);
  float dewPoint = calculateDewPoint(temperature, humidity);
  int humidex = calculateHumidex(temperature, humidity);
  
  // Print formatted values
  printFormattedFloat(temperature, 8);
  Serial.print("│ ");
  printFormattedFloat(humidity, 6);
  Serial.print("│ ");
  printFormattedFloat(heatIndex, 8);
  Serial.print("│ ");
  printFormattedFloat(dewPoint, 6);
  Serial.print("│ ");
  printFormattedInt(humidex, 6);
  Serial.print("│  PASS  │");
}

/**
 * Test outdoor DHT22 sensor
 */
void testOutdoorSensor() {
  Serial.println();
  float humidity = dhtOutdoor.readHumidity();
  float temperature = dhtOutdoor.readTemperature();
  
  Serial.print("│ Outdoor   │ ");
  
  if (isnan(humidity) || isnan(temperature)) {
    Serial.print("   ERROR   │  ERROR │   ERROR  │  ERROR │  ERROR │  FAIL  │");
    outdoorErrors++;
    return;
  }
  
  // Validate readings
  bool tempValid = (temperature >= MIN_TEMP && temperature <= MAX_TEMP);
  bool humValid = (humidity >= MIN_HUMIDITY && humidity <= MAX_HUMIDITY);
  
  if (!tempValid || !humValid) {
    Serial.print("INVALID   │ INVALID│  INVALID │ INVALID│ INVALID│  WARN  │");
    outdoorErrors++;
    return;
  }
  
  // Calculate derived values
  float heatIndex = dhtOutdoor.computeHeatIndex(temperature, humidity, false);
  float dewPoint = calculateDewPoint(temperature, humidity);
  int humidex = calculateHumidex(temperature, humidity);
  
  // Print formatted values
  printFormattedFloat(temperature, 8);
  Serial.print("│ ");
  printFormattedFloat(humidity, 6);
  Serial.print("│ ");
  printFormattedFloat(heatIndex, 8);
  Serial.print("│ ");
  printFormattedFloat(dewPoint, 6);
  Serial.print("│ ");
  printFormattedInt(humidex, 6);
  Serial.print("│  PASS  │");
}

/**
 * Test MQ135 air quality sensor
 */
void testAirQualitySensor() {
  Serial.println();
  int rawValue = analogRead(PIN_AIR_QUALITY);
  
  Serial.print("│ Air Qual  │    --    │   --   │    --    │   --   │   --   │ ");
  
  if (rawValue < MIN_AIR_QUALITY || rawValue > MAX_AIR_QUALITY) {
    Serial.print(" FAIL  │");
    airQualityErrors++;
  } else {
    Serial.print(" PASS  │");
  }
  
  Serial.println();
  Serial.print("│           │          Raw ADC: ");
  printFormattedInt(rawValue, 4);
  Serial.print("   Estimated AQI: ");
  int estimatedAQI = map(rawValue, 0, 1023, 0, 500);
  printFormattedInt(estimatedAQI, 3);
  Serial.print("          │");
}

/**
 * Calculate dew point using Magnus-Tetens formula
 */
float calculateDewPoint(float temp, float humidity) {
  float alpha = log(humidity / 100.0) + (17.27 * temp) / (237.3 + temp);
  return (237.3 * alpha) / (17.27 - alpha);
}

/**
 * Calculate Humidex using Masterton-Richardson equation
 */
int calculateHumidex(float temp, float humidity) {
  float dewPoint = calculateDewPoint(temp, humidity);
  float humidex = temp + 0.5555 * (6.11 * exp(5417.753 * (1/273.16 - 1/(273.15 + dewPoint))) - 10);
  return (int)humidex;
}

/**
 * Print formatted float with specified width
 */
void printFormattedFloat(float value, int width) {
  String str = String(value, 1);
  while (str.length() < width) {
    str = " " + str;
  }
  Serial.print(str);
}

/**
 * Print formatted integer with specified width
 */
void printFormattedInt(int value, int width) {
  String str = String(value);
  while (str.length() < width) {
    str = " " + str;
  }
  Serial.print(str);
}

/**
 * Print summary line
 */
void printSummary() {
  Serial.println();
  Serial.println("├──────────────────────────────────────────────────────────────────────┤");
  Serial.print("│ Total Readings: ");
  printFormattedInt(readingCount, 3);
  Serial.print("  │  Errors - Indoor: ");
  printFormattedInt(indoorErrors, 2);
  Serial.print("  Outdoor: ");
  printFormattedInt(outdoorErrors, 2);
  Serial.print("  AirQual: ");
  printFormattedInt(airQualityErrors, 2);
  Serial.println("     │");
  Serial.println("└──────────────────────────────────────────────────────────────────────┘");
}

/**
 * Print error summary and diagnostic information
 */
void printErrorSummary() {
  Serial.println("\n================= STATUS REPORT =================");
  Serial.print("Uptime: ");
  Serial.print(millis() / 1000);
  Serial.println(" seconds");
  
  float indoorSuccessRate = ((float)(readingCount - indoorErrors) / readingCount) * 100;
  float outdoorSuccessRate = ((float)(readingCount - outdoorErrors) / readingCount) * 100;
  float airQualSuccessRate = ((float)(readingCount - airQualityErrors) / readingCount) * 100;
  
  Serial.print("Indoor sensor success rate: ");
  Serial.print(indoorSuccessRate, 1);
  Serial.println("%");
  
  Serial.print("Outdoor sensor success rate: ");
  Serial.print(outdoorSuccessRate, 1);
  Serial.println("%");
  
  Serial.print("Air quality sensor success rate: ");
  Serial.print(airQualSuccessRate, 1);
  Serial.println("%");
  
  // Diagnostic recommendations
  if (indoorErrors > readingCount * 0.1) {
    Serial.println("⚠️  WARNING: Indoor sensor has high error rate!");
    Serial.println("   Check wiring and power supply to indoor DHT22");
  }
  
  if (outdoorErrors > readingCount * 0.1) {
    Serial.println("⚠️  WARNING: Outdoor sensor has high error rate!");
    Serial.println("   Check wiring and power supply to outdoor DHT22");
  }
  
  if (airQualityErrors > readingCount * 0.1) {
    Serial.println("⚠️  WARNING: Air quality sensor has high error rate!");
    Serial.println("   Check MQ135 connections and allow for burn-in time");
  }
  
  if (indoorErrors == 0 && outdoorErrors == 0 && airQualityErrors == 0) {
    Serial.println("✅ All sensors working perfectly!");
  }
  
  Serial.println("================================================\n");
}