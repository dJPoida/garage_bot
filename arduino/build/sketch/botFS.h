/*============================================================================*\
 * Garage Bot - BotFS
 * Peter Eldred 2021-04
 * 
 * File System wrapper for simplifying interactions with LITTLEFS
\*============================================================================*/
#ifndef BOTFS_H
#define BOTFS_H

#include "Arduino.h"

class BotFS {
  public:
    BotFS();
    bool init();
    bool saveConfig();
    bool resetWiFiConfig(bool enableWiFi);
    bool factoryReset();
    void setWiFiSettings(String newSSID, String newPassword);
    void setIRSensorThreshold(String sensorType, int newThreshold);
    void setMQTTConfig(bool mqttEnabled, String mqttBrokerAddres, unsigned int mqttBrokerPort, String mqttDeviceId, String mqttUsername, String mqttPassword, String mqttTopic, String mqttStateTopic);
    void setNetworkConfig(String mdnsName, String networkDeviceName);
    void registerRFCode(unsigned long newCode);

  private:
    bool loadConfig();

    bool _writingConfig;
};

extern BotFS botFS;

#endif
