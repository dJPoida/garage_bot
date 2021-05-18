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

#include "wifiEngine.h"
#include "Arduino.h"
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <LITTLEFS.h>
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "_config.h"
#include "helpers.h"
#include "wifiCaptivePortalHandler.h"
#include "botFS.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"


/**
 * Constructor
 */
WiFiEngine::WiFiEngine(){}


/**
 * Initialise
 */
bool WiFiEngine::init(AsyncWebServer *webServer, AsyncWebSocket *webSocket, DNSServer *dnsServer) {
  #ifdef SERIAL_DEBUG
  Serial.println("Initialising WiFi engine...");
  #endif

  // Keep a pointer to the AsyncWebServer, AsyncWebSocket and DNS Server
  _webServer = webServer;
  _webSocket = webSocket;
  _dnsServer = dnsServer;

  // At this point we can consider ourselves uninitialised
  wifiEngineMode = WEM_UNINIT;
  
  // attempt to connect to the configured wifi 
  if (!connectToHotSpot()) {
    // Attempt to serve up the dedicated Access Point
    if (!broadcastAP()) {
      // Don't continue if unable to perform either connection
      return false;
    }
  }

  // Some default headers for handling CORS problems in javascript
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  
  // Setup the routes
  initRoutes();

  // If we're in Access Point Mode, Assign the Captive WebRequestHandler
  if (wifiEngineMode == WEM_AP) {
    _webServer->addHandler(new WiFiCaptivePortalHandler()).setFilter(ON_AP_FILTER);
  
    // Start the DNS server for the CaptiveRequestHandler
    _dnsServer->start(53, "*", WiFi.softAPIP());
  }
  
  // Start the Web Server
  _webServer->begin();

  #ifdef SERIAL_DEBUG
  Serial.println("WiFi engine initialised.\n");
  #endif

  return true;
}


/**
 * Attempt to connect to the configured WiFi hotspot
 */
bool WiFiEngine::connectToHotSpot() {
  if (WiFi.status() != WL_CONNECTED) {
    // Don't bother if we don't have a configuration yet
    if (!config.wifi_ssid.equals("")) {
      #ifdef SERIAL_DEBUG
      Serial.print(" - Connecting to: "); 
      Serial.println(config.wifi_ssid);
      #endif

      // Set the host name so that other devices recognise this device as the garage bot
      WiFi.hostname(config.network_device_name.c_str());
      WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);

      // Attempt to connect to the configured wifi hotspot
      WiFi.begin(config.wifi_ssid.c_str(), config.wifi_password.c_str()); 

      // Wait for the connect result
      if (WiFi.waitForConnectResult() != WL_CONNECTED) {
          #ifdef SERIAL_DEBUG
          Serial.println(" ! Failed to connect!");
          #endif
          return false;
      }       

      // Start the mDNS service to broadcast the device's location on the network
      if(!MDNS.begin(config.mdns_name.c_str())) {
        #ifdef SERIAL_DEBUG
        Serial.println(" ! Error starting mDNS!");
        #endif
        return false;
      }
      
      wifiEngineMode = WEM_CLIENT;
      ipAddress = WiFi.localIP().toString();
      macAddress = WiFi.macAddress();
      config.ip_address = ipAddress;
    
      #ifdef SERIAL_DEBUG
      Serial.print(" - WiFi connected."); 
      Serial.print(" - IP address: "); 
      Serial.println(ipAddress);
      Serial.print(" - MAC address: ");
      Serial.println(macAddress);
      Serial.print(" - Device name: ");
      Serial.println(config.network_device_name);
      Serial.print(" - Web address: http://");
      Serial.print(config.mdns_name);
      Serial.println(".local");
      #endif

      return true;
    }
  }

  return false;
}


/**
 * Attempt to broadcast a dedicated Access Point
 */
bool WiFiEngine::broadcastAP() {
  #ifdef SERIAL_DEBUG
  Serial.println(" - Attempting to Broadcast dedicated Access Point...");
  #endif

  WiFi.mode(WIFI_AP_STA);
  
  if (!WiFi.softAP(AP_SSID)) {
    // The adapter was not able to set the desired SSID for the acces point
    #ifdef SERIAL_DEBUG
    Serial.println(" ! Failed to assign the softAP SSID.");
    #endif
    return false;
  }

  wifiEngineMode = WEM_AP;
  ipAddress = WiFi.softAPIP().toString();
  macAddress = WiFi.softAPmacAddress();
 
  #ifdef SERIAL_DEBUG
  Serial.print(" - Access Point broadcasting on SSID: '");
  Serial.print(AP_SSID);
  Serial.println("'");
  Serial.print(" - IP address: ");
  Serial.println(ipAddress);
  Serial.print(" - MAC address: ");
  Serial.println(macAddress);
  #endif

  return true;
}


/**
 * Used when serving HTML files to replace key variables in the HTML with
 * current state data.
 */
String WiFiEngine::templateProcessor(const String& var){
  #ifdef SERIAL_DEBUG
  Serial.println(var);
  #endif

  if (var == "FIRMWARE_VERSION") {
    return FIRMWARE_VERSION;
  } else if (var == "DEVICE_ADDRESS") {
    return config.ip_address;
  }

  return var;
}


/**
 * Initialise the routes for serving the web app content
 */
void WiFiEngine::initRoutes() {
  _webSocket->onEvent([&](AsyncWebSocket *ws, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
    onWsEvent(ws, client, type, arg, data, len);
  });
  _webServer->addHandler(_webSocket);
  
  // Route for root / web page (apmode.html)
  _webServer->on("/", HTTP_GET, [&](AsyncWebServerRequest *request){
    if (wifiEngineMode == WEM_AP) {
      Serial.println("Sending APMODE");
      request->send(LITTLEFS, "/apmode.html", String(), false, templateProcessor);
    } else {
      Serial.println("Sending INDEX");
      request->send(LITTLEFS, "/index.html", String(), false, templateProcessor);
    }
  });

  // Set the wifi access point details
  _webServer->on("/setwifi", HTTP_POST, [&](AsyncWebServerRequest *request){}, NULL, [&](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if (request->url() == "/setwifi") {
      handleSetWiFi(request, data);
    }
  });

  // All other Files / Routes
  _webServer->onNotFound([](AsyncWebServerRequest *request){
    // Attempt to load the file from the LITTLEFS file system
    if ((request->method() == HTTP_GET) && LITTLEFS.exists(request->url())) {
      request->send(LITTLEFS, request->url(), getMimeType(request->url()));
    }

    // Handle HTTP_OPTIONS
    else if (request->method() == HTTP_OPTIONS) {
      request->send(200);
    }

    // Return a 404 if the file wasn't found
    else {
      #ifdef SERIAL_DEBUG
      Serial.print(" - 404: ");
      Serial.print(request->methodToString());
      Serial.print(" '");
      Serial.print(request->url());
      Serial.println("'");
      #endif
      request->send(404, "text/plain", "The content you are looking for was not found.");
    }
  });
}


/**
 * Fired when a webSocket event occurs
 */
void WiFiEngine::onWsEvent(AsyncWebSocket *ws, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  // Fired when a client connects to the websocket
  if (type == WS_EVT_CONNECT){
      #ifdef SERIAL_DEBUG
      Serial.println("Websocket client connection received");
      #endif
      _client = client;
      sendConfigToClients();
  }

  // Fired when a websocket client disconnects
  else if (type == WS_EVT_DISCONNECT){
      #ifdef SERIAL_DEBUG
      Serial.println("Websocket client connection finished");
      #endif
      _client = NULL;
  }

  // Fired when a websocket client sends data
  else if (type == WS_EVT_DATA) {
    
  }
}


/**
 * Send the device config to connected clients.
 * Typically happens just after connection and when the config changes.
 */
void WiFiEngine::sendConfigToClients() {
  if (!_client != NULL && (_client->status() == WS_CONNECTED)) {
    
    
    // TODO: stringify the config
    _client->text("boobs");
  }
}


/**
 * Handles setting new WiFi connection details
 */
void WiFiEngine::handleSetWiFi(AsyncWebServerRequest *request, uint8_t *body){
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, (const char*)body);
  
  String wifiSSID = doc["wifiSSID"];
  String wifiPassword = doc["wifiPassword"];

  botFS.setWiFiSettings(wifiSSID, wifiPassword);
            
  request->send(200, "text/json", "{\"success\":true}");
}


/**
 * Handle a websocket message
 */
void WiFiEngine::handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    const uint8_t size = JSON_OBJECT_SIZE(1);
    StaticJsonDocument<size> json;
    DeserializationError err = deserializeJson(json, data);
    if (err) {
      #ifdef SERIAL_DEBUG
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(err.c_str());
      #endif
      return;
    }

    const char *action = json["action"];

    // Open
    if (strcmp(action, "open") == 0) {
      #ifdef SERIAL_DEBUG
      Serial.println("TODO: Do something with the open action");
      #endif
    }
  }
}
