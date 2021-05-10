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
    bool resetWiFiConfig();
    void setWiFiSettings(String newSSID, String newPassword);

  private:
    bool loadConfig();

    bool _writingConfig;
};

extern BotFS botFS;

#endif
