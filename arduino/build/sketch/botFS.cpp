/*============================================================================*\
 * Garage Bot - BotFS
 * Peter Eldred 2021-04
 * 
 * File System wrapper for simplifying interactions with LITTLEFS
\*============================================================================*/

#include "Arduino.h"
#include "ArduinoJson.h"
#include "LITTLEFS.h"
#include "_config.h"
#include "botFS.h"
#include "reboot.h"


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
    Serial.println("  ! Failed to mount file system");
    #endif
    return false;
  }
  
  // Give LITTLEFS a tick to catch up
  delay(250);
  
  #ifdef SERIAL_DEBUG
  Serial.println("  - LITTLEFS initialised.");
  #endif

  // Load the config from the onboard SPI File System
  if (!loadConfig()) {
    #ifdef SERIAL_DEBUG
    Serial.println("  - Creating new Config File... ");
    #endif
  }

  // Save the config back to the SPI File System
  if (!saveConfig()) {
    #ifdef SERIAL_DEBUG
    Serial.println("  ! Failed to create new config file on LITTLEFS.");
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
    Serial.println("  ! Failed to open config file");
    #endif
    return false;
  }

  size_t size = configFile.size();
  if (size > CONFIG_FILE_MAX_SIZE) {
    #ifdef SERIAL_DEBUG
    Serial.println("  ! Error - config file size is too large");
    #endif
    return false;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<CONFIG_FILE_MAX_SIZE> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, configFile);

  #ifdef SERIAL_DEBUG
  if (error) {
    Serial.println("  ! Failed to read file, using default configuration: ");
  } else {
    Serial.println("  - Config loaded from 'LITTLEFS/config.json': ");
  }
  #endif

  // Update the global variables from the json doc
  config.mdns_name = doc["mdns_name"] | config.mdns_name;
  config.device_name = doc["device_name"] | config.device_name;
  JsonVariant wifiEnabled = doc["wifi_enabled"];
  config.wifi_enabled = wifiEnabled.isNull() ? config.wifi_enabled : wifiEnabled.as<bool>();
  config.wifi_ssid = doc["wifi_ssid"] | config.wifi_ssid;
  config.wifi_password = doc["wifi_password"] | config.wifi_password;
  JsonVariant mqttEnabled = doc["mqtt_enabled"];
  config.mqtt_enabled = mqttEnabled.isNull() ? config.mqtt_enabled : mqttEnabled.as<bool>();
  config.mqtt_broker_address = doc["mqtt_broker_address"] | config.mqtt_broker_address;
  JsonVariant mqttPort = doc["mqtt_broker_port"];
  config.mqtt_broker_port = mqttPort.isNull() ? config.mqtt_broker_port : mqttPort.as<int>();
  config.mqtt_device_id = doc["mqtt_device_id"] | config.mqtt_device_id;
  config.mqtt_username = doc["mqtt_username"] | config.mqtt_username;
  config.mqtt_password = doc["mqtt_password"] | config.mqtt_password;
  config.mqtt_command_topic = doc["mqtt_command_topic"] | config.mqtt_command_topic;
  config.mqtt_state_topic = doc["mqtt_state_topic"] | config.mqtt_state_topic;
  config.stored_rf_code_count = doc["stored_rf_code_count"] | config.stored_rf_code_count;
  JsonArray rfCodes = doc["rf_codes"];
  for (byte i = 0; i < 5; i++) {
    config.rf_codes[i] = rfCodes.getElement(i);
  }
  JsonVariant topIrSensorThreshold = doc["top_ir_sensor_threshold"];
  config.top_ir_sensor_threshold = topIrSensorThreshold.isNull() ? config.top_ir_sensor_threshold : topIrSensorThreshold.as<int>();
  JsonVariant bottomIrSensorThreshold = doc["bottom_ir_sensor_threshold"];
  config.bottom_ir_sensor_threshold = bottomIrSensorThreshold.isNull() ? config.bottom_ir_sensor_threshold : bottomIrSensorThreshold.as<int>();

  if (rfCodes) {
    copyArray(rfCodes, config.rf_codes);
  }

  configFile.close();
    
  #ifdef SERIAL_DEBUG
  Serial.println("  - Current Config:");
  Serial.print("    + mDNS Name: ");
  Serial.println(config.mdns_name);
  Serial.print("    + Device Name: ");
  Serial.println(config.device_name);
  Serial.print("    + WiFi: ");
  Serial.println(config.wifi_enabled ? "Enabled" : "Disabled");
  if (config.wifi_enabled) {
    Serial.print("    + WiFi SSID: ");
    Serial.println(config.wifi_ssid);
    Serial.print("    + WiFi Password: ");
    Serial.println(config.wifi_password);
  }
  Serial.print("    + ");
  Serial.print(config.stored_rf_code_count);
  Serial.println(" Registered RF Codes: ");
  for (byte i = 0; i < config.stored_rf_code_count; i++) {
    Serial.print("      '");
    Serial.print(config.rf_codes[i]);
    Serial.println("'");
  }
  Serial.print("    + MQTT: ");
  Serial.println(config.mqtt_enabled ? "Enabled" : "Disabled");
  if (config.mqtt_enabled) {
    Serial.print("    - Broker Address: ");
    Serial.println(config.mqtt_broker_address);
    Serial.print("    - Broker Port: ");
    Serial.println(config.mqtt_broker_port);
    Serial.print("    - Device ID: ");
    Serial.println(config.mqtt_device_id);
    Serial.print("    - Username: ");
    Serial.println(config.mqtt_username);
    Serial.print("    - Password: ");
    Serial.println(config.mqtt_password);
    Serial.print("    - Topic: ");
    Serial.println(config.mqtt_command_topic);
    Serial.print("    - State Topic: ");
    Serial.println(config.mqtt_state_topic);
  }
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
  Serial.println("  - Writing configuration to 'LITTLEFS/config.json'...");
  #endif

  // Open file for writing
  File configFile = LITTLEFS.open("/config.json", "w");
  if (!configFile) {
    #ifdef SERIAL_DEBUG
    Serial.println("  ! Failed to create 'LITTLEFS/config.json'!");
    #endif
    return false;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<CONFIG_FILE_MAX_SIZE> doc;

  // Set the values in the document
  doc["mdns_name"]                  = config.mdns_name;
  doc["device_name"]                = config.device_name;
  doc["wifi_enabled"]               = config.wifi_enabled;
  doc["wifi_ssid"]                  = config.wifi_ssid;
  doc["wifi_password"]              = config.wifi_password;
  doc["mqtt_enabled"]               = config.mqtt_enabled;
  doc["mqtt_broker_address"]        = config.mqtt_broker_address;
  doc["mqtt_broker_port"]           = config.mqtt_broker_port;
  doc["mqtt_device_id"]             = config.mqtt_device_id;
  doc["mqtt_username"]              = config.mqtt_username;
  doc["mqtt_password"]              = config.mqtt_password;
  doc["mqtt_command_topic"]         = config.mqtt_command_topic;
  doc["mqtt_state_topic"]           = config.mqtt_state_topic;
  doc["stored_rf_code_count"]       = config.stored_rf_code_count;
  doc["top_ir_sensor_threshold"]    = config.top_ir_sensor_threshold;
  doc["bottom_ir_sensor_threshold"] = config.bottom_ir_sensor_threshold;
  JsonArray rfCodes = doc.createNestedArray("rf_codes");
  for (byte i = 0; i < 5; i++) {
    rfCodes.add(config.rf_codes[i]);
  }

  // Serialize JSON to file
  if (serializeJson(doc, configFile) == 0) {
    #ifdef SERIAL_DEBUG
    Serial.println("  ! Failed to write to 'LITTLEFS/config.json'");
    #endif
  }

  // Close the file
  configFile.close();

  _writingConfig = false;
  
  return true;
}


/**
 * Reset the config for the WiFi Hotspot
 * 
 * @param {bool} enableWiFi whether to enable the WiFi or not
 */
void BotFS::resetWiFiConfig(bool enableWiFi) {
  #ifdef SERIAL_DEBUG
  Serial.println(enableWiFi ? "Resetting (and/or enabling) WiFi Config..." : "Disabling WiFi...");
  #endif

  config.wifi_enabled = enableWiFi;
  config.wifi_ssid = "";
  config.wifi_password = "";
  
  saveConfig();

  reboot();
}


/**
 * Reset the entire config to the defaults (just delete it from the file system)
 */
void BotFS::factoryReset() {
  // Wait for any current write operations to finish
  while (_writingConfig) {
    delay(1);
  };
  _writingConfig = true;
  
  #ifdef SERIAL_DEBUG
  Serial.println("Factory Reset:");
  Serial.println("  - Deleteing 'LITTLEFS/config.json'...");
  #endif

  // Delete the file
  LITTLEFS.remove("/config.json");

  #ifdef SERIAL_DEBUG
  Serial.println("  - Done");
  #endif

  _writingConfig = false;
  
  reboot();
}


/**
 * Change the current Wifi Settings (triggered from the Configuration Website)
 * 
 * @param String newSSID the new WiFi SSID to save to the device
 * @param String newPassword the new WiFi Password to save to the device
 */
void BotFS::setWiFiSettings(String newSSID, String newPassword) {
    config.wifi_ssid = newSSID;
    config.wifi_password = newPassword;

    #ifdef SERIAL_DEBUG
    Serial.print("Configuring and saving new WiFi Hotspot details, SSID: '");
    Serial.print(newSSID);
    Serial.print("', Password: '");
    Serial.print(newPassword);
    Serial.println("'...");
    #endif

    // Save the updated config.
    saveConfig();

    // Reboot the device
    reboot();
}


/**
 * Change the current Wifi Settings (triggered from the Configuration Website)
 * 
 * @param String mdnsName The name to use in the mdns address that clients can use to connect to the device without the IP (i.e. http://garagebot.local)
 * @param String deviceName The device name to display to other devices on the network
 * @param bool mqttEnabled Whether the device should attempt to integrate with an MQTT broker
 * @param String mqttBrokerAddres The IP address of the MQTT Broker
 * @param unsigned int mqttBrokerPort The MQTT Broker Port Number
 * @param String mqttDeviceId The Device ID to use when connecting to the MQTT Broker
 * @param String mqttUsername The username when connecting to the MQTT broker
 * @param String mqttPassword The password when connecting to the MQTT broker
 * @param String mqttCommandTopic The MQTT topic used for communicating instructions (open / close etc)
 * @param String mqttStateTopic The MQTT topic used for communicating the state of the door (opened / closed / etc)
 */
void BotFS::setGeneralConfig(
  String mdnsName,
  String deviceName,
  bool mqttEnabled,
  String mqttBrokerAddres,
  unsigned int mqttBrokerPort,
  String mqttDeviceId,
  String mqttUsername,
  String mqttPassword,
  String mqttCommandTopic,
  String mqttStateTopic
) {
  config.mdns_name = mdnsName;
  config.device_name = deviceName;
  config.mqtt_enabled = mqttEnabled;
  config.mqtt_broker_address = mqttBrokerAddres;
  config.mqtt_broker_port = mqttBrokerPort;
  config.mqtt_device_id = mqttDeviceId;
  config.mqtt_username = mqttUsername;
  config.mqtt_password = mqttPassword;
  config.mqtt_command_topic = mqttCommandTopic;
  config.mqtt_state_topic = mqttStateTopic;
  
  #ifdef SERIAL_DEBUG
  Serial.println("Configuring and saving General Config:");
  Serial.print("  - MDNS name: '");
  Serial.println(config.mdns_name);
  Serial.print("  - Device Name: '");
  Serial.println(config.device_name);
  Serial.print("  + MQTT: ");
  Serial.println(config.mqtt_enabled ? "Enabled" : "Disabled");

  if (config.mqtt_enabled) {
    Serial.print("    - Broker Address: ");
    Serial.println(config.mqtt_broker_address);
    Serial.print("    - Broker Port: ");
    Serial.println(config.mqtt_broker_port);
    Serial.print("    - Device ID: ");
    Serial.println(config.mqtt_device_id);
    Serial.print("    - Username: ");
    Serial.println(config.mqtt_username);
    Serial.print("    - Password: ");
    Serial.println(config.mqtt_password);
    Serial.print("    - Topic: ");
    Serial.println(config.mqtt_command_topic);
    Serial.print("    - State Topic: ");
    Serial.println(config.mqtt_state_topic);
  }
  #endif

  // Save the updated config.
  saveConfig();

  // Reboot the device
  reboot();
}


/**
 * Register a new RF Code
 */
void BotFS::registerRFCode(unsigned long newCode) {
  // Bail if the Stored RF Codes exceed the maximum
  if (config.stored_rf_code_count >= 5) {
    return;
  }

  // Increment the Stored RF code count
  config.stored_rf_code_count += 1;

  #ifdef SERIAL_DEBUG
  Serial.print("Storing RF code #");
  Serial.print(config.stored_rf_code_count);
  Serial.print(": '");
  Serial.print(newCode);
  Serial.println("'");
  #endif

  // Store the new code into the config
  config.rf_codes[config.stored_rf_code_count - 1] = newCode;

  // Save the updated config
  saveConfig();

  // Reboot the device
  reboot();
}


/**
 * Change the threshold of an IR Sensor
 * 
 * @param String sensorType "TOP" or "BOTTOM"
 * @param int newThreshold the new threshold to store in the config
 */
void BotFS::setIRSensorThreshold(String sensorType, int newThreshold) {
  if (sensorType == "TOP") {
    config.top_ir_sensor_threshold = newThreshold;
  } else if (sensorType == "BOTTOM") {
    config.bottom_ir_sensor_threshold = newThreshold;
  }

  // Save the updated config.
  saveConfig();
}