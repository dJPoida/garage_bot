/*============================================================================*\
 * Garage Bot - mqttClient
 * Peter Eldred 2021-05
 * 
 * This class is designed to connect to a MQTT broker and issue commands or
 * subscribe to topics 
\*============================================================================*/

#include "_config.h"
#include "mqttClient.h"
#include "wifiEngine.h"
#include "doorControl.h"


/**
 * This gets around the "error: invalid use of non-static member function" that I'm
 * not smart enough to circumvent yet.
 */
void staticHandleMessageReceived(char* topic, byte* data, unsigned int len) {
  mqttClient.handleMessageReceived(topic, data, len);
}


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

  setMQTTState(MQTT_STATE_DISCONNECTED, "");

  // Check each of the configuration requirements
  if (config.mqtt_broker_address.equals("")) {
    setMQTTState(MQTT_STATE_CONFIG_ERROR, "No MQTT Broker Address configured.");
  } else if (config.mqtt_broker_port <= 0 || config.mqtt_broker_port >= 65535) {
    setMQTTState(MQTT_STATE_CONFIG_ERROR, "Invalid MQTT Broker Port configured.");
  } else if (config.mqtt_device_id.equals("")) {
    setMQTTState(MQTT_STATE_CONFIG_ERROR, "No MQTT Device ID configured.");
  } else if (config.mqtt_command_topic.equals("")) {
    setMQTTState(MQTT_STATE_CONFIG_ERROR, "No MQTT Command Topic configured.");
  } else if (config.mqtt_state_topic.equals("")) {
    setMQTTState(MQTT_STATE_CONFIG_ERROR, "No MQTT State Topic configured.");
  } else {
    // Calculate the unique device ID by concatenating the last four digits from the mac address with the garage bot prefix
    deviceId = config.mqtt_device_id + "_" + wifiEngine.macAddress.substring(wifiEngine.macAddress.length() - 5, wifiEngine.macAddress.length() - 3) + wifiEngine.macAddress.substring(wifiEngine.macAddress.length() - 2);

    // Connect to the MQTT broker  
    _pubSubClient->setServer(config.mqtt_broker_address.c_str(), config.mqtt_broker_port);
    _pubSubClient->setCallback(staticHandleMessageReceived);  
    connectToBroker();
  }

  #ifdef SERIAL_DEBUG
  Serial.println("MQTT Client initialised.\n");
  #endif
}


/**
 * Set the state of the client and notify any listeners
 * @param {MQTTState} newState the new state to set
 * @param {String} error an optional error
 */
void MQTTClient::setMQTTState(MQTTState newState, String error) {
  _mqttState = newState;
  _error = error;

  if (onStateChange) {
    onStateChange(_mqttState, _error);
  };
}


/**
 * Attempt to connect to the configured MQTT Broker
 */
bool MQTTClient::connectToBroker() {
  #ifdef SERIAL_DEBUG
  Serial.print("  - Connecting to MQTT Broker: "); 
  Serial.print(config.mqtt_broker_address);
  Serial.print(":");
  Serial.println(config.mqtt_broker_port);
  Serial.print("  - Device ID: "); 
  Serial.println(deviceId);
  #endif

  bool connectSuccessful = false;

  // Attempt to connect with credentials
  if (!config.mqtt_username.equals("")) {
    #ifdef SERIAL_DEBUG
    Serial.print("  - Username: "); 
    Serial.println(config.mqtt_username);
    Serial.print("  - Password: "); 
    Serial.println(config.mqtt_password);
    #endif
    connectSuccessful = _pubSubClient->connect(deviceId.c_str(), config.mqtt_username.c_str(), config.mqtt_password.c_str());
  } 
  
  // Attempt to connect without credentials
  else {
    connectSuccessful = _pubSubClient->connect(deviceId.c_str());
  }
  
  if (connectSuccessful) {
    // Once connected, publish the current state (messages OUT)
    _pubSubClient->publish(config.mqtt_state_topic.c_str(), doorControl.getDoorStateAsString().c_str());
    // resubscribe to the command topic (messages IN)
    _pubSubClient->subscribe(config.mqtt_command_topic.c_str());

    return true;
  } else {
    #ifdef SERIAL_DEBUG
    Serial.print("  ! Failed to connect to MQTT Broker: "); 
    Serial.println(_getPubSubClientStateAsString());
    #endif
    setMQTTState(_getPubSubClientStateAsEnum(), "");

    return false;
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

  #ifdef SERIAL_DEBUG
  Serial.println("MQTT Message Received: ");
  Serial.print("  - Topic: ");
  Serial.println(topic);
  Serial.print("  - Message: ");
  Serial.println(message);
  #endif

  // Open command
  if (strcmp(message, "open") == 0) {
    doorControl.open();
  }

  // Close command
  else if (strcmp(message, "close") == 0) {
    doorControl.close();
  }

  // Activate command
  else if (strcmp(message, "activate") == 0) {
    doorControl.activate();
  }
}


/**
 * Convert the current pubsubclient MQTT Client state into an enumerated value for our use
 */
MQTTState MQTTClient::_getPubSubClientStateAsEnum() {
  switch (_pubSubClient->state()) {
    case MQTT_CONNECTION_TIMEOUT:
      return MQTT_STATE_CONNECTION_TIMEOUT;
    case MQTT_CONNECTION_LOST:
      return MQTT_STATE_CONNECTION_LOST;
    case MQTT_CONNECT_FAILED:
      return MQTT_STATE_CONNECT_FAILED;
    case MQTT_DISCONNECTED:
      return MQTT_STATE_DISCONNECTED;
    case MQTT_CONNECT_BAD_PROTOCOL:
      return MQTT_STATE_CONNECT_BAD_PROTOCOL;
    case MQTT_CONNECT_BAD_CLIENT_ID:
      return MQTT_STATE_CONNECT_BAD_CLIENT_ID;
    case MQTT_CONNECT_UNAVAILABLE:
      return MQTT_STATE_CONNECT_UNAVAILABLE;
    case MQTT_CONNECT_BAD_CREDENTIALS:
      return MQTT_STATE_CONNECT_BAD_CREDENTIALS;
    case MQTT_CONNECT_UNAUTHORIZED:
      return MQTT_STATE_CONNECT_UNAUTHORIZED;
    default:
      return MQTT_STATE_CONNECTED;
  }
}


/**
 * Convert the current MQTT PubSubClient state into a string for transport to the client
 */
String MQTTClient::_getPubSubClientStateAsString() {
  switch (_pubSubClient->state()) {
    case MQTT_CONNECTION_TIMEOUT:
      return F("MQTT_CONNECTION_TIMEOUT");
    case MQTT_CONNECTION_LOST:
      return F("MQTT_CONNECTION_LOST");
    case MQTT_CONNECT_FAILED:
      return F("MQTT_CONNECT_FAILED");
    case MQTT_DISCONNECTED:
      return F("MQTT_DISCONNECTED");
    case MQTT_CONNECTED:
      return F("MQTT_CONNECTED");
    case MQTT_CONNECT_BAD_PROTOCOL:
      return F("MQTT_CONNECT_BAD_PROTOCOL");
    case MQTT_CONNECT_BAD_CLIENT_ID:
      return F("MQTT_CONNECT_BAD_CLIENT_ID");
    case MQTT_CONNECT_UNAVAILABLE:
      return F("MQTT_CONNECT_UNAVAILABLE");
    case MQTT_CONNECT_BAD_CREDENTIALS:
      return F("MQTT_CONNECT_BAD_CREDENTIALS");
    case MQTT_CONNECT_UNAUTHORIZED:
      return F("MQTT_CONNECT_UNAUTHORIZED");
    default:
      return F("UNKNOWN");
  }
}


/**
 * Convert the current MQTT state into a string for transport to the client
 */
String MQTTClient::getMQTTStateAsString() {
  switch (_mqttState) {
    case MQTT_STATE_CONNECTION_TIMEOUT:
      return F("MQTT_STATE_CONNECTION_TIMEOUT");
    case MQTT_STATE_CONNECTION_LOST:
      return F("MQTT_STATE_CONNECTION_LOST");
    case MQTT_STATE_CONNECT_FAILED:
      return F("MQTT_STATE_CONNECT_FAILED");
    case MQTT_STATE_DISCONNECTED:
      return F("MQTT_STATE_DISCONNECTED");
    case MQTT_STATE_CONNECTED:
      return F("MQTT_STATE_CONNECTED");
    case MQTT_STATE_CONNECT_BAD_PROTOCOL:
      return F("MQTT_STATE_CONNECT_BAD_PROTOCOL");
    case MQTT_STATE_CONNECT_BAD_CLIENT_ID:
      return F("MQTT_STATE_CONNECT_BAD_CLIENT_ID");
    case MQTT_STATE_CONNECT_UNAVAILABLE:
      return F("MQTT_STATE_CONNECT_UNAVAILABLE");
    case MQTT_STATE_CONNECT_BAD_CREDENTIALS:
      return F("MQTT_STATE_CONNECT_BAD_CREDENTIALS");
    case MQTT_STATE_CONNECT_UNAUTHORIZED:
      return F("MQTT_STATE_CONNECT_UNAUTHORIZED");

    case MQTT_STATE_CONFIG_ERROR:
      return F("MQTT_STATE_CONFIG_ERROR");
    case MQTT_STATE_DISABLED:
      return F("MQTT_STATE_DISABLED");
    default:
      return F("UNKNOWN");
  }
}


/**
 * Get the MQTT Client State
 */
MQTTState MQTTClient::getMQTTState() {
  return _mqttState;
}


/**
 * Get the MQTT Client Error (if any)
 */
String MQTTClient::getMQTTError() {
  return _error;
}


/**
 * run
 *
 * Called on a regular basis by the main loop to Check the connectivity to the MQTT client
 * and run other ongoing MQTT tasks
 *
 * @param currentMillis the current milliseconds as passed down from the main loop
 */
void MQTTClient::run (unsigned long currentMillis) {
  // Has the MQTT connection dropped?
  if (!_pubSubClient->connected()) {
    // If the reconnect interval has passed AND the wifi client is connected...
    if (wifiEngine.connected && (currentMillis - _lastReconnectAttempt > RECONNECT_INTERVAL)) {
      _lastReconnectAttempt = currentMillis;
      // Attempt to reconnect
      if (connectToBroker()) {
        // Successful connection
        _lastReconnectAttempt = 0;
      }
      setMQTTState(_getPubSubClientStateAsEnum(), "");
    }
  }
  
  // Don't run the main loop code unless the MQTT client is enabled and not in error
  else {
    // Allow the pubSubClient to perform it's main loop code
    _pubSubClient->loop();

    // Make sure our knowledge of the MQTT state is up to date
    if (_getPubSubClientStateAsEnum() != _mqttState) {
      setMQTTState(_getPubSubClientStateAsEnum(), "");
    }
  }
}


/**
 * Send the current door state to the MQTT broker
 * Triggered by a change in the door state
 */
void MQTTClient::sendDoorStateToBroker() {
  if (_pubSubClient->connected()) {
    String doorState = doorControl.getDoorStateAsString();

    #ifdef SERIAL_DEBUG
    Serial.print("Sending Door State to MQTT Broker: ");
    Serial.println(doorState);
    #endif

    doorState.toLowerCase();
    _pubSubClient->publish(config.mqtt_state_topic.c_str(), doorState.c_str());
  }
}