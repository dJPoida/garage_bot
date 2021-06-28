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
    mqttClientStateChangedFunction onStateChange;

    MQTTClientState getMQTTClientState();
    String getMQTTClientError();
    String getMQTTClientStateAsString();
  private:
    PubSubClient *_pubSubClient;                  // A pointer to the PubSubClient passed into the init function
    
    MQTTClientState _clientState = MQTT_STATE_DISABLED; // The current state of the MQTT Client
    String _clientError = "";                     // Any error that the MQTT Client may have encountered
    unsigned long _lastReconnectAttempt = 0;      // the millis() that the MQTT client last attempted to connect to the configured MQTT Broker

    void setClientState(MQTTClientState newState, String error);  // Set the state of the MQTT client with an optional error
    bool connectToBroker();                       // Connect to the MQTT Broker
    void handleMessageReceived(char* topic, byte* payload, unsigned int length); // Message received from the MQTT broker
};

extern MQTTClient mqttClient;

#endif
