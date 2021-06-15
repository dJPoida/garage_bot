/*============================================================================*\
 * Garage Bot - reboot
 * Peter Eldred 2021-04
 * 
 * Simple wrapper for functions that help the device gracefully reboot
\*============================================================================*/

#include "LITTLEFS.h"
#include "_config.h"
#include "reboot.h"
#include "wifiEngine.h"

// Flag to tell the kernel it's time for a reset
bool rebootFlag = false;


/**
 * Flags the kernal down for a reset
 */
void reboot() {
  #ifdef SERIAL_DEBUG
  Serial.println("rebooting...");
  #endif

  rebootFlag = true;
}


/**
 * If flagged, reboots the device by pulling the reset pin high
 * Don't call this directly. Instead, use `reboot()` which
 * sets a flag that is checked on the next loop.
 */
void checkReboot() {
  if (rebootFlag) {
    // Notify clients
    #ifdef SERIAL_DEBUG
    Serial.println("Notifying connected clients of impending reboot...");
    #endif
    wifiEngine.sendRebootingToClients();
    delay(200);

    #ifdef SERIAL_DEBUG
    Serial.println("Shutting Down LITTLEFS...");
    #endif
    LITTLEFS.end();
    delay(200);
  
    #ifdef SERIAL_DEBUG
    Serial.println("Restarting device...");
    #endif
    ESP.restart();
  }
}
