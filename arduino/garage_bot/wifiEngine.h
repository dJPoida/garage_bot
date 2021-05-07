/*============================================================================*\
 * Garage Bot - WifiClient
 * Peter Eldred 2021-04
 * 
 * This unit controls all of the Garage Bot's connectivity.
 *
 * When the Garage Bot is configured to connect to a WiFi hotspot, it serves
 * up the app webpage.
 * 
 * When it cannot connect to a WiFi hotspot, it serves up the AccessPoint
 * config webpage.
\*============================================================================*/

#ifndef WIFIENGINE_H
#define WIFIENGINE_H

#include <DNSServer.h>
#include "ESPAsyncWebServer.h"
#include "AsyncTCP.h"

// Used to keep track of the mode we're in
enum WiFiEngineMode {
  WEM_UNINIT,   // Uninitialised
  WEM_CLIENT,   // Connected to the Configured WiFi hotspot as a Client
  WEM_AP        // Serving a dedicated Access Point
};

class WiFiEngine {
  public:
    WiFiEngine();
    bool init(AsyncWebServer *webServer, DNSServer *dnsServer);

    WiFiEngineMode wifiEngineMode = WEM_UNINIT;   // The current mode of the WiFi engine (uninitialised, client or AP mode)
    String ipAddress;                             // The IP address of the active wifi connection
    String macAddress;                            // The MAC accress of the wifi adaptor

  private:
    AsyncWebServer *_webServer;                   // A pointer to the web server passed into the init function
    DNSServer *_dnsServer;                        // A pointer to the dns server passed into the init function
    
    bool connectToHotSpot();                      // Connect to the configured hot spot and put the device in client mode
    bool broadcastAP();                           // Broadcast the Access Point putting the device in AP mode

    static String templateProcessor(const String& var);   // Used when serving HTML files to replace key variables in the HTML
    
    void initRoutes();                                    // Initialise the AP mode Web Server routes
    
    void handleSetWiFi(AsyncWebServerRequest *request);   // Handle calls to set the WiFi Access Point
};

extern WiFiEngine wifiEngine;

#endif
