/*============================================================================*\
 * Garage Bot - mqttClient
 * Peter Eldred 2021-05
 * 
 * This class is designed to connect to a MQTT broker and issue commands or
 * subscribe to topics 
\*============================================================================*/

#include "mqttClient.h"

/**
 * Constructor
 */
MQTTClient::MQTTClient(){}


/**
 * Initialise
 */
void MQTTClient::init() {
  #ifdef SERIAL_DEBUG
  Serial.print("Initialising MQTT Client...");
  #endif
  
  #ifdef SERIAL_DEBUG
  Serial.println(" done.");
  #endif  
}
