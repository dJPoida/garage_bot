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
#include "helpers.h"

class WiFiEngine {
  public:
    WiFiEngine();
    bool init(AsyncWebServer *webServer, AsyncWebSocket *webSocket, DNSServer *dnsServer);

    WiFiEngineMode wifiEngineMode = WEM_UNINIT;               // The current mode of the WiFi engine (uninitialised, client or AP mode)
    String ipAddress;                                         // The IP address of the active wifi connection
    String macAddress;                                        // The MAC accress of the wifi adaptor

    void sendConfigToClients(AsyncWebSocketClient *client = NULL);  // Send the current device config to (a) connected client(s)
    void sendStatusToClients(AsyncWebSocketClient *client = NULL);  // Send the current device status to (a) connected client(s)

  private:
    AsyncWebServer *_webServer;                   // A pointer to the web server passed into the init function
    AsyncWebSocket *_webSocket;                   // A pointer to the web socket passed into the init function
    DNSServer *_dnsServer;                        // A pointer to the dns server passed into the init function

    bool connectToHotSpot();                      // Connect to the configured hot spot and put the device in client mode
    bool broadcastAP();                           // Broadcast the Access Point putting the device in AP mode

    static String templateProcessor(const String& var);   // Used when serving HTML files to replace key variables in the HTML
    
    void initRoutes();                            // Initialise the AP mode Web Server routes

    void onWsEvent(AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len); // Handle websocket events
    void handleWebSocketData(void *arg, uint8_t *data, size_t len);     // Handle a websocket data message
    
    void handleSetWiFi(AsyncWebServerRequest *request, uint8_t *body);  // Handle calls to set the WiFi Access Point
};

extern WiFiEngine wifiEngine;

#endif
