/**
 * @file strings.h
 * @brief Multilingual strings for LCD display
 * 
 * Contains all text strings used in the LCD display in multiple languages.
 * Language selection is done via LANGUAGE define in config.h
 * 
 * Supported languages:
 * - French (LANG_FR)
 * - English (LANG_EN)
 * 
 * @author F. Baillon
 * @version 1.0.0
 * @date January 2025
 * @license MIT License
 * 
 * Copyright (c) 2025 F. Baillon
 */

#ifndef STRINGS_H
#define STRINGS_H

#include "config.h"

// ==========================================
// MULTILINGUAL STRING DEFINITIONS
// ==========================================

// Select strings based on configured language
#if LANGUAGE == LANG_FR
  // French strings
  #define STR_PROJECT_NAME      "  SMART LED CLOCK   "
  #define STR_VERSION           "   VERSION 1.0.0    "
  
  // Startup messages
  #define STR_INITIALIZING      "INITIALISATION..."
  #define STR_BUTTON_READY      "BOUTON PRET"
  #define STR_LEDS_READY        "LEDS PRETES"
  #define STR_SENSORS_READY     "CAPTEURS PRETS"
  #define STR_DS3231_READY      "DS3231 PRET"
  #define STR_DS3231_ERROR      "DS3231 ERREUR!"
  #define STR_CONNECTING_WIFI   "CONNEXION WIFI..."
  #define STR_WIFI_CONNECTED    "WIFi CONNECTE"
  #define STR_NO_WIFI           "PAS DE WiFi >>> RTC"
  #define STR_SYNCING_TIME      "SYNCHRO TEMPS..."
  #define STR_TIME_SYNCED       "HEURE SYNCHRONISEE"
  #define STR_USING_RTC_TIME    "HEURE RTC UTILISEE"
  #define STR_INITIALIZING      "INIT SERVEUR WEB..."
  #define STR_LOAD_CONFIG       "CHARGEMENT CONFIG..."
  #define STR_READING_SENSORS   "LECTURE CAPTEURS..."
  #define STR_SYSTEM_READY      "   SYSTEME PRET!"
  
  // Display labels
  #define STR_TIME              "Heure:"
  #define STR_INDOOR_SHORT      "Int:"
  #define STR_OUTDOOR_SHORT     "Ext:"
  #define STR_ERROR             "ERREUR"
  #define STR_AQI               "AQI:"
  
  // Day names (short form)
  #define STR_DAY_0             "DIM"  // Sunday
  #define STR_DAY_1             "LUN"  // Monday
  #define STR_DAY_2             "MAR"  // Tuesday
  #define STR_DAY_3             "MER"  // Wednesday
  #define STR_DAY_4             "JEU"  // Thursday
  #define STR_DAY_5             "VEN"  // Friday
  #define STR_DAY_6             "SAM"  // Saturday
  
  // Month names (short form) - currently not used in display
  #define STR_MONTH_1           "JANV"
  #define STR_MONTH_2           "FEV"
  #define STR_MONTH_3           "MARS"
  #define STR_MONTH_4           "AVR"
  #define STR_MONTH_5           "MAI"
  #define STR_MONTH_6           "JUIN"
  #define STR_MONTH_7           "JUIL"
  #define STR_MONTH_8           "AOUT"
  #define STR_MONTH_9           "SEPT"
  #define STR_MONTH_10          "OCT"
  #define STR_MONTH_11          "NOV"
  #define STR_MONTH_12          "DEC"
  
  // Mode 2: Feels-like temperature
  #define STR_FEELS_LIKE_TITLE  "Temp. Ressentie"
  #define STR_OUTDOOR           "Exterieur:"
  #define STR_FEELS_LIKE        "Ressenti :"
  #define STR_DEW_POINT         "Pt rosee :"
  
  // Mode 3: Humidex
  #define STR_HUMIDEX_TITLE     "Indice Humidex"
  #define STR_HUMIDEX           "Humidex:"
  #define STR_OUTDOOR_ONLY      "Exterieur uniquement"
  
  // Humidex comfort levels
  #define STR_HUMIDEX_NONE      "  PAS D'INCONFORT   "
  #define STR_HUMIDEX_LITTLE    "  PEU D'INCONFORT   "
  #define STR_HUMIDEX_SOME      " INCONFORT CERTAIN  "
  #define STR_HUMIDEX_GREAT     " EVITER LES EFFORTS "
  #define STR_HUMIDEX_DANGER    "DANGER COUP CHALEUR "
  
  // Animation
  #define STR_HOURLY_ANIMATION  "  Animation horaire"
  
#elif LANGUAGE == LANG_EN
  // English strings
  #define STR_PROJECT_NAME      "Smart LED Clock"
  #define STR_VERSION           "Phase 5 - Final"
  
  // Startup messages
  #define STR_INITIALIZING      "Initializing..."
  #define STR_BUTTON_READY      "Button Ready"
  #define STR_LEDS_READY        "LEDs Ready"
  #define STR_SENSORS_READY     "Sensors Ready"
  #define STR_DS3231_READY      "DS3231 Ready"
  #define STR_DS3231_ERROR      "DS3231 ERROR!"
  #define STR_CONNECTING_WIFI   "Connecting WiFi..."
  #define STR_WIFI_CONNECTED    "WiFi Connected"
  #define STR_NO_WIFI           "No WiFi"
  #define STR_SYNCING_TIME      "Syncing time..."
  #define STR_TIME_SYNCED       "Time Synchronized"
  #define STR_USING_RTC_TIME    "Using RTC time"
  #define STR_READING_SENSORS   "Reading sensors..."
  #define STR_SYSTEM_READY      "System Ready!"
  
  // Display labels
  #define STR_TIME              "Time:"
  #define STR_INDOOR_SHORT      "In:"
  #define STR_OUTDOOR_SHORT     "Out:"
  #define STR_ERROR             "ERROR"
  #define STR_AQI               "AQI:"
  
  // Day names (short form)
  #define STR_DAY_0             "Sun"  // Sunday
  #define STR_DAY_1             "Mon"  // Monday
  #define STR_DAY_2             "Tue"  // Tuesday
  #define STR_DAY_3             "Wed"  // Wednesday
  #define STR_DAY_4             "Thu"  // Thursday
  #define STR_DAY_5             "Fri"  // Friday
  #define STR_DAY_6             "Sat"  // Saturday
  
  // Month names (short form) - currently not used in display
  #define STR_MONTH_1           "Jan"
  #define STR_MONTH_2           "Feb"
  #define STR_MONTH_3           "Mar"
  #define STR_MONTH_4           "Apr"
  #define STR_MONTH_5           "May"
  #define STR_MONTH_6           "Jun"
  #define STR_MONTH_7           "Jul"
  #define STR_MONTH_8           "Aug"
  #define STR_MONTH_9           "Sep"
  #define STR_MONTH_10          "Oct"
  #define STR_MONTH_11          "Nov"
  #define STR_MONTH_12          "Dec"
  
  // Mode 2: Feels-like temperature
  #define STR_FEELS_LIKE_TITLE  "Feels-like Temp"
  #define STR_OUTDOOR           "Outdoor  :"
  #define STR_FEELS_LIKE        "Feels-like:"
  #define STR_DEW_POINT         "Dew point:"
  
  // Mode 3: Humidex
  #define STR_HUMIDEX_TITLE     "Humidex Index"
  #define STR_HUMIDEX           "Humidex:"
  #define STR_OUTDOOR_ONLY      "Outdoor only"
  
  // Humidex comfort levels
  #define STR_HUMIDEX_NONE      "No discomfort"
  #define STR_HUMIDEX_LITTLE    "Little discomfort"
  #define STR_HUMIDEX_SOME      "Some discomfort"
  #define STR_HUMIDEX_GREAT     "Avoid exertion"
  #define STR_HUMIDEX_DANGER    "Heat danger"
  
  // Animation
  #define STR_HOURLY_ANIMATION  "  Hourly animation"
  
#else
  #error "Unsupported language selected in config.h"
#endif

// ==========================================
// HELPER FUNCTION
// ==========================================

/**
 * @brief Get day name for given day of week
 * @param dayOfWeek Day index (0=Sunday, 6=Saturday)
 * @return Pointer to day name string
 */
inline const char* getDayName(uint8_t dayOfWeek) {
  static const char* days[] = {
    STR_DAY_0, STR_DAY_1, STR_DAY_2, STR_DAY_3,
    STR_DAY_4, STR_DAY_5, STR_DAY_6
  };
  return (dayOfWeek < 7) ? days[dayOfWeek] : "";
}

/**
 * @brief Get month name for given month
 * @param month Month index (1=January, 12=December)
 * @return Pointer to month name string
 */
inline const char* getMonthName(uint8_t month) {
  static const char* months[] = {
    "", // Index 0 unused
    STR_MONTH_1, STR_MONTH_2, STR_MONTH_3, STR_MONTH_4,
    STR_MONTH_5, STR_MONTH_6, STR_MONTH_7, STR_MONTH_8,
    STR_MONTH_9, STR_MONTH_10, STR_MONTH_11, STR_MONTH_12
  };
  return (month >= 1 && month <= 12) ? months[month] : "";
}

/**
 * @brief Get humidex comfort description
 * 
 * Converts humidex value to comfort description.
 * 
 * Scale (Canadian standard):
 * - < 20: No discomfort
 * - 20-29: Little discomfort
 * - 30-39: Some discomfort
 * - 40-44: Great discomfort, avoid exertion
 * - ≥ 45: Dangerous, heat stroke risk
 * 
 * @param humidex Humidex value (unitless index)
 * @return Pointer to description string
 */
inline const char* getHumidexString(int humidex) {
  if (humidex < 20) return STR_HUMIDEX_NONE;
  if (humidex < 30) return STR_HUMIDEX_LITTLE;
  if (humidex < 40) return STR_HUMIDEX_SOME;
  if (humidex < 45) return STR_HUMIDEX_GREAT;
  return STR_HUMIDEX_DANGER;
}

#endif // STRINGS_H