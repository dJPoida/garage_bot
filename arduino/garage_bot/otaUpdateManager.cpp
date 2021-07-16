/*============================================================================*\
 * Garage Bot - otaUpdateManager
 * Peter Eldred 2021-07
 * 
 * This class handles Over The Air (OTA) updates for the garage bot
\*============================================================================*/

#include "_config.h"
#include "otaUpdateManager.h"
#include "ArduinoOTA.h"

/**
 * Constructor
 */
OTAUpdateManager::OTAUpdateManager(){}


/**
 * Initialise
 * @note: this will only be called if WiFi is enabled
 * @note: most of this code was copied from the BasicOTA.ino ESP32 example
 */
void OTAUpdateManager::init() {
  #ifdef SERIAL_DEBUG
  Serial.println("Initialising OTA Update Manager...");
  #endif

  ArduinoOTA.setHostname(config.device_name.c_str());
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  #ifdef SERIAL_DEBUG
  Serial.println("OTA Update Manager initialised.\n");
  #endif
}


/**
 * run
 *
 * Called on a regular basis by the main loop
 *
 * @param currentMillis the current milliseconds as passed down from the main loop
 */
void OTAUpdateManager::run (unsigned long currentMillis) {
  ArduinoOTA.handle();
}
