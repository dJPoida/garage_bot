/*============================================================================*\
 * Garage Bot - mqttClient
 * Peter Eldred 2021-05
 * 
 * This class is designed to connect to a MQTT broker and issue commands or
 * subscribe to topics 
\*============================================================================*/

#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include "helpers.h"
#include "PubSubClient.h"

class MQTTClient {
  public:
    MQTTClient();
    
    void init(PubSubClient *pubSubClient);
    mqttStateChangedFunction onStateChange;

    MQTTState getMQTTState();
    String getMQTTError();
    String getMQTTStateAsString();

    String deviceId;                              // The configure device id concatenated with the device MAC address to generate a unique ID for the MQTT broker

    void run (unsigned long currentMillis);       // Fired every time the main loop on the arduino program is fired
    void handleMessageReceived(char* topic, byte* payload, unsigned int length); // Message received from the MQTT broker
    void sendDoorStateToBroker();                 // Send the current door state to the MQTT broker
  private:
    PubSubClient *_pubSubClient;                  // A pointer to the PubSubClient passed into the init function

    MQTTState _mqttState = MQTT_STATE_DISABLED;   // The current state of the MQTT Client (including our last known state)
    String _error = "";                           // Any error that the MQTT Client may have encountered
    unsigned long _lastReconnectAttempt = 0;      // the millis() that the MQTT client last attempted to connect to the configured MQTT Broker

    void setMQTTState(MQTTState newState, String error);  // Set the known state of the MQTT client with an optional error
    bool connectToBroker();                       // Connect to the MQTT Broker

    String _getPubSubClientStateAsString();
    MQTTState _getPubSubClientStateAsEnum();
};

extern MQTTClient mqttClient;

#endif
