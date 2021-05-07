/*============================================================================*\
 * Garage Bot - BotFS
 * Peter Eldred 2021-04
 * 
 * File System wrapper for simplifying interactions with LITTLEFS
\*============================================================================*/
#ifndef BOTFS_H
#define BOTFS_H

class BotFS {
  public:
    BotFS();
    bool init();
    bool saveConfig();
    bool resetWiFiConfig();

  private:
    bool loadConfig();

    bool _writingConfig;
};

extern BotFS botFS;

#endif
