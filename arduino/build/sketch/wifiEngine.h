/*============================================================================*\
 * Garage Bot - WiFiEngine
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

#ifndef WIFI_ENGINE_H
#define WIFI_ENGINE_H

#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "DNSServer.h"
#include "helpers.h"

class WiFiEngine {
  public:
    WiFiEngine();
    bool init(AsyncWebServer *webServer, AsyncWebSocket *webSocket, DNSServer *dnsServer);

    WiFiEngineMode wifiEngineMode = WEM_UNINIT;               // The current mode of the WiFi engine (uninitialised, client or AP mode)
    bool connected = false;                                   // Whether the WiFi client is connected to the configured hotspot
    String ipAddress;                                         // The IP address of the active wifi connection
    String macAddress;                                        // The MAC address of the wifi adaptor

    boolValueChangedFunction onConnectedChanged;              // Fired when connected changes from true to false etc...

    void sendConfigToClients(AsyncWebSocketClient *client = NULL);  // Send the current device config to (a) connected client(s)
    void sendStatusToClients(AsyncWebSocketClient *client = NULL);  // Send the current device status to (a) connected client(s)
    
    void run (
      unsigned long currentMillis,
      boolean topIRSensorDetected,
      int topIRSensorAverageAmbientReading,
      int topIRSensorAverageActiveReading,
      boolean bottomIRSensorDetected,
      int bottomIRSensorAverageAmbientReading,
      int bottomIRSensorAverageActiveReading
    );                                                        // Send sensor data to connected web socket clients

  private:
    AsyncWebServer *_webServer;                   // A pointer to the web server passed into the init function
    AsyncWebSocket *_webSocket;                   // A pointer to the web socket passed into the init function
    DNSServer *_dnsServer;                        // A pointer to the dns server passed into the init function

    unsigned long _lastSensorBroadcast = 0;       // the millis() that the sensor data was last broadcast to connected socket clients
    unsigned long _lastReconnectAttempt = 0;      // the millis() that the WiFi client last attempted to connect to the configured access point

    bool connectToHotSpot();                      // Connect to the configured hot spot and put the device in client mode
    bool broadcastAP();                           // Broadcast the Access Point putting the device in AP mode
    void _handleWiFiConnected();                  // processes actions required after the device connects to the configured WiFi access point
    void _handleWiFiDisconnected();               // processes actions required after the device is disconnected from the configured WiFi access point

    static String templateProcessor(const String& var);   // Used when serving HTML files to replace key variables in the HTML
    
    void initRoutes();                            // Initialise the AP mode Web Server routes

    void onWsEvent(AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len); // Handle websocket events
    void handleWebSocketData(void *arg, uint8_t *data, size_t len);     // Handle a websocket data message
    
    void handleSetWiFi(AsyncWebServerRequest *request, uint8_t *body);  // Handle calls to set the WiFi Access Point
};

extern WiFiEngine wifiEngine;

#endif
