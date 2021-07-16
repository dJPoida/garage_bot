/*============================================================================*\
 * Garage Bot - otaUpdateManager
 * Peter Eldred 2021-07
 * 
 * This class handles Over The Air (OTA) updates for the garage bot
\*============================================================================*/

#ifndef OTA_UPDATE_MANAGER_H
#define OTA_UPDATE_MANAGER_H

class OTAUpdateManager {
  public:
    OTAUpdateManager();
    
    void init();
    void run (unsigned long currentMillis);       // Fired every time the main loop on the arduino program is fired
};

extern OTAUpdateManager otaUpdateManager;

#endif