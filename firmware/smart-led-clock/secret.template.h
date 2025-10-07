/**
 * @file secrets.template.h
 * @brief WiFi credentials template
 * 
 * INSTRUCTIONS:
 * 1. Copy this file to "secrets.h"
 * 2. Create a "secrets.cpp" with this content:
 * 
 *    #include "secrets.h"
 *    const char* ssid = "YourSSID";
 *    const char* pass = "YourPassword";
 * 
 * 3. Replace YourSSID and YourPassword with your actual credentials
 * 4. DO NOT commit secrets.h or secrets.cpp to version control
 * 
 * @author F. Baillon
 * @version 1.0.0
 * @date January 2025
 * @license MIT License
 */

#ifndef SECRETS_H
#define SECRETS_H

// WiFi network credentials
extern const char* ssid;
extern const char* pass;

#endif // SECRETS_H