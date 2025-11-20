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
 * @version 1.1.0
 * @date November 2025
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
  // ----------------
  // LCD messages
  // ----------------
  // Startup messages
  #define STR_PROJECT_NAME      "  SMART LED CLOCK   "
  #define STR_VERSION           "   VERSION 1.2.0    "
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
  #define STR_INIT_WEB_SERVER   "INIT SERVEUR WEB..."
  #define STR_LOAD_CONFIG       "CHARGEMENT CONFIG..."
  #define STR_READING_SENSORS   "LECTURE CAPTEURS..."
  #define STR_INIT_MOON_PHASE   "INIT PHASE LUNE..."
  #define STR_MOON_PHASE_READY  "PHASE LUNE PRET"
  #define STR_MOON_PHASE_ERROR  "PHASE LUNE ERREUR"
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
  #define STR_FEELS_LIKE_TITLE  "  TEMP. RESSENTIE   "
  #define STR_OUTDOOR           "EXTERIEURE: "
  #define STR_FEELS_LIKE        " RESSENTIE: "
  #define STR_DEW_POINT         "  PT ROSEE: "
  
  // Mode 3: Humidex
  #define STR_HUMIDEX_TITLE     "   INDICE HUMIDEX   "
  #define STR_HUMIDEX_ERROR     "       ERREUR       "
  #define STR_OUTDOOR_ONLY      "EXTERIEUR UNIQUEMENT"
  
  // Humidex comfort levels
  #define STR_HUMIDEX_NONE      "  PAS D'INCONFORT   "
  #define STR_HUMIDEX_LITTLE    "  PEU D'INCONFORT   "
  #define STR_HUMIDEX_SOME      " INCONFORT CERTAIN  "
  #define STR_HUMIDEX_GREAT     " EVITER LES EFFORTS "
  #define STR_HUMIDEX_DANGER    "DANGER COUP CHALEUR "
  
  // Animation
  #define STR_HOURLY_ANIMATION  " ANIMATION HORAIRE  "

  // Moon animation
  #define STR_MOON_CALIB_MSG1   "  ORIENTEZ SPHERE   "
  #define STR_MOON_CALIB_MSG2   " FACE NOIRE VISIBLE "
  #define STR_MOON_CALIB_MSG3   "                    "
  #define STR_MOON_CALIB_MSG4   " APPUYEZ SUR BOUTON "
  #define STR_MOON_ORIENT_OK    "ORIENTATION OK"
  #define STR_MOON_COMPUT_PHASE "CALCUL PHASE LUNE..."
  #define STR_MOON_POSITION     "POSITION LUNE..."
  #define STR_MOON_POSITIONED   "LUNE POSITIONNEE"
  #define STR_MOON_POSI_ERROR   "POSITION ERREUR"
  #define STR_MOON_CALIBRATION  "CALIBRATION LUNE..."
  #define STR_MOON_CALIBR_ERROR "CALIBRATION ERREUR"

  // Other messages
  // -----------------  
  // Air quality
  #define STR_AIR_QA_GOOD           "Bon"
  #define STR_AIR_QA_MODERATE       "Modéré"
  #define STR_AIR_QA_UNHEALTHY_SG   "Mauvais PS"
  #define STR_AIR_QA_UNHEALTHY      "Mauvais"
  #define STR_AIR_QA_VERY_POOR      "Très mauvais"
  #define STR_AIR_QA_HAZARDOUS      "Dangereux"

  #define MOON_PHASE_NEW_MOON         "Nouvelle lune"
  #define MOON_PHASE_WAXING_CRESCENT  "Premier croissant"
  #define MOON_PHASE_FIRST_QUARTER    "Premier quartier"
  #define MOON_PHASE_WAXING_GIBBUS    "Gibbeuse croissante"
  #define MOON_PHASE_FULL_MOON        "Pleine lune"
  #define MOON_PHASE_WANING_GIBBUS    "Gibbeuse décroissante"
  #define MOON_PHASE_LAST_QUARTER     "Dernier quartier"
  #define MOON_PHASE_WANING_CRESCENT  "Dernier croissant"

#elif LANGUAGE == LANG_EN
  // English strings
  // ----------------
  // LCD messages
  // ----------------
  // Startup messages
  #define STR_PROJECT_NAME      "  SMART LED CLOCK   "
  #define STR_VERSION           "   VERSION 1.2.0    "
  #define STR_INITIALIZING      "INITIALIZING..."
  #define STR_BUTTON_READY      "BUTTON_READY"
  #define STR_LEDS_READY        "LEDS_READY"
  #define STR_SENSORS_READY     "SENSORS_READY"
  #define STR_DS3231_READY      "DS3231_READY"
  #define STR_DS3231_ERROR      "DS3231 ERROR!"
  #define STR_CONNECTING_WIFI   "CONNECTING_WIFI..."
  #define STR_WIFI_CONNECTED    "WIFI_CONNECTED"
  #define STR_NO_WIFI           "NO_WIFI"
  #define STR_SYNCING_TIME      "SYNCING_TIME..."
  #define STR_TIME_SYNCED       "TIME SYNCHRONIZES"
  #define STR_USING_RTC_TIME    "USING RTC TIME"
  #define STR_READING_SENSORS   "READING_SENSORS..."
  #define STR_SYSTEM_READY      "SYSTEM_READY!"
  
  // Display labels
  #define STR_TIME              "TIME:"
  #define STR_INDOOR_SHORT      "IN:"
  #define STR_OUTDOOR_SHORT     "OUT:"
  #define STR_ERROR             "ERROR"
  #define STR_AQI               "AQI:"
  
  // Day names (short form)
  #define STR_DAY_0             "SUN"  // Sunday
  #define STR_DAY_1             "MON"  // Monday
  #define STR_DAY_2             "TUE"  // Tuesday
  #define STR_DAY_3             "WED"  // Wednesday
  #define STR_DAY_4             "THU"  // Thursday
  #define STR_DAY_5             "FRI"  // Friday
  #define STR_DAY_6             "SAT"  // Saturday
  
  // Month names (short form) - currently not used in display
  #define STR_MONTH_1           "JAN"
  #define STR_MONTH_2           "FEB"
  #define STR_MONTH_3           "MAR"
  #define STR_MONTH_4           "APR"
  #define STR_MONTH_5           "MAY"
  #define STR_MONTH_6           "JUN"
  #define STR_MONTH_7           "JUL"
  #define STR_MONTH_8           "AUG"
  #define STR_MONTH_9           "SEP"
  #define STR_MONTH_10          "OCT"
  #define STR_MONTH_11          "NOV"
  #define STR_MONTH_12          "DEC"
  
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

  // Moon animation
  #define STR_MOON_CALIB_MSG1   "  ORIENTEZ SPHERE   "
  #define STR_MOON_CALIB_MSG2   " FACE NOIRE VISIBLE "
  #define STR_MOON_CALIB_MSG3   "                    "
  #define STR_MOON_CALIB_MSG4   " APPUYEZ SUR BOUTON "
  #define STR_MOON_ORIENT_OK    "ORIENTATION OK"
  #define STR_MOON_COMPUT_PHASE "CALCUL PHASE LUNE..."
  #define STR_MOON_POSITION     "POSITION LUNE..."
  #define STR_MOON_POSITIONED   "LUNE POSITIONNEE"
  #define STR_MOON_POSI_ERROR   "POSITION ERREUR"
  #define STR_MOON_CALIBRATION  "CALIBRATION LUNE..."
  #define STR_MOON_CALIBR_ERROR "CALIBRATION ERREUR"

  // Other messages
  // -----------------  
  // Air quality
  #define STR_AIR_QA_GOOD           "Good"
  #define STR_AIR_QA_MODERATE       "Moderate"
  #define STR_AIR_QA_UNHEALTHY_SG   "Unhealthy SG"
  #define STR_AIR_QA_UNHEALTHY      "Unhealthy"
  #define STR_AIR_QA_VERY_POOR      "Very poor"
  #define STR_AIR_QA_HAZARDOUS      "Hazardous"

  #define MOON_PHASE_NEW_MOON         "New Moon"
  #define MOON_PHASE_WAXING_CRESCENT  "Waxing Crescent"
  #define MOON_PHASE_FIRST_QUARTER    "First Quarter"
  #define MOON_PHASE_WAXING_GIBBUS    "Waxing Gibbous"
  #define MOON_PHASE_FULL_MOON        "Full Moon"
  #define MOON_PHASE_WANING_GIBBUS    "Waning Gibbous"
  #define MOON_PHASE_LAST_QUARTER     "Last Quarter"
  #define MOON_PHASE_WANING_CRESCENT  "Waning Crescent"
  
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