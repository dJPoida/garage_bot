/*============================================================================*\
 * Garage Bot - mqttClient
 * Peter Eldred 2021-05
 * 
 * This class is designed to connect to a MQTT broker and issue commands or
 * subscribe to topics 
\*============================================================================*/

#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include "helpers.h"

class MQTTClient {
  public:
    MQTTClient();
    
    void init();
  private:
};

extern MQTTClient mqttClient;

#endif
