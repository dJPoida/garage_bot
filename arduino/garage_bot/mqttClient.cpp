/*============================================================================*\
 * Garage Bot - mqttClient
 * Peter Eldred 2021-05
 * 
 * This class is designed to connect to a MQTT broker and issue commands or
 * subscribe to topics 
\*============================================================================*/

#include "_config.h"
#include "mqttClient.h"

/**
 * Constructor
 */
MQTTClient::MQTTClient(){}


/**
 * Initialise
 * @note: this will only be called if MQTT is enabled
 */
void MQTTClient::init(PubSubClient *pubSubClient) {
  #ifdef SERIAL_DEBUG
  Serial.println("Initialising MQTT Client...");
  #endif

  // Keep a pointer to some of the important global objects
  _pubSubClient = pubSubClient;

  // Start off as disconnected
  _clientState = MQTT_STATE_DISCONNECTED;

  // Check each of the configuration requirements
  if (config.mqtt_broker_address.equals("")) {
    setClientState(MQTT_STATE_ERROR, "No MQTT Broker Address configured.");
  } else if (config.mqtt_broker_port <= 0 || config.mqtt_broker_port >= 65535) {
    setClientState(MQTT_STATE_ERROR, "Invalid MQTT Broker Port configured.");
  } else if (config.mqtt_device_id.equals("")) {
    setClientState(MQTT_STATE_ERROR, "No MQTT Device ID configured.");
  } else if (config.mqtt_topic.equals("")) {
    setClientState(MQTT_STATE_ERROR, "No MQTT Topic configured.");
  } else if (config.mqtt_state_topic.equals("")) {
    setClientState(MQTT_STATE_ERROR, "No MQTT State Topic configured.");
  } else {
    // Connect to the MQTT broker  
    // client.setServer(server, 1883);
    // client.setCallback(callback);  
    connectToBroker();
  }

  #ifdef SERIAL_DEBUG
  Serial.println("MQTT Client initialised.\n");
  #endif
}


/**
 * Set the state of the client and notify any listeners
 * @param {MQTTClientState} newState the new state to set
 * @param {String} error an optional error
 */
void MQTTClient::setClientState(MQTTClientState newState, String error = "") {
  _clientState = newState;
  _clientError = error;

  if (onStateChange) {
    onStateChange(_clientState, _clientError);
  };
}


/**
 * Attempt to connect to the configured MQTT Broker
 */
bool MQTTClient::connectToBroker() {
  // Do we have all of the information we need to connect to an MQTT broker?
  if (!config.mqtt_broker_address.equals("")) {
    #ifdef SERIAL_DEBUG
    Serial.print("  - Connecting to MQTT Broker: "); 
    Serial.println(config.mqtt_broker_address);
    #endif
  }
}


/**
 * Fired when the MQTT Client receives a message from the MQTT broker
 */
void MQTTClient::handleMessageReceived(char* topic, byte* data, unsigned int len) {
  // Let's assume the message is a string.
  char message[MQTT_MAX_PACKET_SIZE];
  memcpy( message, data, len );
  message[len] = '\0';

  // TODO: handle MQTT message
  #ifdef SERIAL_DEBUG
  Serial.println("MQTT Message Received: ");
  Serial.print("  - Topic: ");
  Serial.println(topic);
  Serial.print("  - Message: ");
  Serial.println(message);
  #endif
}


/**
 * Convert the current MQTT Client state into a string for transport to the client
 */
String MQTTClient::getMQTTClientStateAsString() {
  switch (_clientState) {
    case MQTT_STATE_ERROR:
      return "ERROR";
    case MQTT_STATE_CONNECTING:
      return "CONNECTING";
    case MQTT_STATE_CONNECTED:
      return "CONNECTED";
    case MQTT_STATE_DISCONNECTED:
      return "DISCONNECTED";
    default: 
      return "DISABLED";
  }
}


/**
 * Get the MQTT Client State
 */
MQTTClientState MQTTClient::getMQTTClientState() {
  return _clientState;
}


/**
 * Get the MQTT Client Error (if any)
 */
String MQTTClient::getMQTTClientError() {
  return _clientError;
}
