/*============================================================================*\
 * Garage Bot - BotFS
 * Peter Eldred 2021-04
 * 
 * File System wrapper for simplifying interactions with LITTLEFS
\*============================================================================*/

#include "botFS.h"
#include "Arduino.h"
#include "_config.h"
#include <LITTLEFS.h>
#include <ArduinoJson.h>


/**
 * Constructor
 */
BotFS::BotFS(){
  _writingConfig = false; 
}


/**
 * Initialise
 */
bool BotFS::init() {
  #ifdef SERIAL_DEBUG
  Serial.println("Initialising BotFS...");
  #endif

  // Start the SPI Flash Files System
  if (!LITTLEFS.begin(true)) {
    #ifdef SERIAL_DEBUG
    Serial.println(" ! Failed to mount file system");
    #endif
    return false;
  }
  
  // Give LITTLEFS a tick to catch up
  delay(250);
  
  #ifdef SERIAL_DEBUG
  Serial.println(" - LITTLEFS initialised.");
  #endif

  // Load the config from the onboard SPI File System
  if (!loadConfig()) {
    #ifdef SERIAL_DEBUG
    Serial.println(" - Creating new Config File... ");
    #endif
  }

  // Save the config back to the SPI File System
  if (!saveConfig()) {
    #ifdef SERIAL_DEBUG
    Serial.println(" ! Failed to create new config file on LITTLEFS.");
    #endif
    
    return false;
  }

  #ifdef SERIAL_DEBUG
  Serial.println("BotFS initialised.\n");
  #endif

  return true;
}


/**
 * Open up the config.json file on the LITTLEFS partition and store the milky goodness within
 */
bool BotFS::loadConfig() {
  File configFile = LITTLEFS.open("/config.json", "r");
  if (!configFile) {
    #ifdef SERIAL_DEBUG
    Serial.println(" ! Failed to open config file");
    #endif
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    #ifdef SERIAL_DEBUG
    Serial.println(" ! Error - config file size is too large");
    #endif
    return false;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<1024> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, configFile);

  if (error) {
    #ifdef SERIAL_DEBUG
    Serial.println(" ! Failed to read file, using default configuration: ");
    #endif
  } else {
    #ifdef SERIAL_DEBUG
    Serial.println(" - Config loaded from 'LITTLEFS/config.json': ");
    #endif
  }

  // Update the global variables from the json doc
  config.wifi_ssid = doc["wifi_ssid"] | config.wifi_ssid;
  config.wifi_password = doc["wifi_pw"] | config.wifi_password;

  configFile.close();
    
  #ifdef SERIAL_DEBUG
  Serial.println(" - Current Config:");
  Serial.print("   + WiFi SSID: ");
  Serial.println(config.wifi_ssid);
  Serial.print("   + WiFi Password: ");
  Serial.println(config.wifi_password);
  #endif
    
  return true;
}


/**
 * Save the current configuration to LITTLEFS
 */
bool BotFS::saveConfig() {
  // Wait for any current write operations to finish
  while (_writingConfig) {
    delay(1);
  };
  _writingConfig = true;
  
  #ifdef SERIAL_DEBUG
  Serial.println(" - Writing configuration to 'LITTLEFS/config.json'...");
  #endif

  // Open file for writing
  File configFile = LITTLEFS.open("/config.json", "w");
  if (!configFile) {
    #ifdef SERIAL_DEBUG
    Serial.println(" ! Failed to create 'LITTLEFS/config.json'!");
    #endif
    return false;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<1024> doc;

  // Set the values in the document
  doc["wifi_ssid"]  = config.wifi_ssid;
  doc["wifi_pw"]    = config.wifi_password;
  doc["ip_address"] = config.ip_address;

  // Serialize JSON to file
  if (serializeJson(doc, configFile) == 0) {
    #ifdef SERIAL_DEBUG
    Serial.println(" ! Failed to write to 'LITTLEFS/config.json'");
    #endif
  }

  // Close the file
  configFile.close();

  _writingConfig = false;
  
  return true;
}


/**
 * Reset the config for the WiFi Hotspot
 */
bool BotFS::resetWiFiConfig() {
  #ifdef SERIAL_DEBUG
  Serial.println("Resetting WiFi Config...");
  #endif

  config.wifi_ssid = "";
  config.wifi_password = "";
  config.ip_address = "";
  
  saveConfig();
}
