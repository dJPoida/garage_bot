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

#include "Arduino.h"
#include "DNSServer.h"
#include "ESPmDNS.h"
#include "LITTLEFS.h"
#include "WiFi.h"
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "_config.h"
#include "helpers.h"
#include "wifiEngine.h"
#include "wifiCaptivePortalHandler.h"
#include "botFS.h"
#include "doorControl.h"
#include "mqttClient.h"
#include "irsensor.h"
#include "reboot.h"

/**
 * Constructor
 */
WiFiEngine::WiFiEngine(){}


/**
 * Initialise
 */
bool WiFiEngine::init(AsyncWebServer *webServer, AsyncWebSocket *webSocket, DNSServer *dnsServer, IRSensor *topIRSensor, IRSensor *bottomIRSensor) {
  #ifdef SERIAL_DEBUG
  Serial.println("Initialising WiFi engine...");
  #endif

  // Keep a pointer to some of the important global objects
  _webServer = webServer;
  _webSocket = webSocket;
  _dnsServer = dnsServer;
  _topIRSensor = topIRSensor;
  _bottomIRSensor = bottomIRSensor;

  // At this point we can consider ourselves uninitialized
  wifiEngineMode = WEM_UNINIT;
  
  // Has the WiFi hotspot been configured?
  if (!config.wifi_ssid.equals("")) {
    connectToHotSpot();
  }

  else {
    // Attempt to serve up the dedicated Access Point
    if (!broadcastAP()) {
      // Don't continue if unable to broadcast an Access Point
      return false;
    }
  }
  
  // Some default headers for handling CORS problems in javascript
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "content-type");
  
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
 * Once the device has been initialised and the WiFi is connected, incoming websockets will be allowed
 */
void WiFiEngine::allowIncomingWebSockets() {
  #ifdef SERIAL_DEBUG
  Serial.println("Incoming WebSocket connections enabled.");
  #endif

  _webSocket->onEvent([&](AsyncWebSocket *ws, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
    onWsEvent(client, type, arg, data, len);
  });
  _webServer->addHandler(_webSocket);
}


/**
 * Attempt to connect to the configured WiFi hotspot
 */
bool WiFiEngine::connectToHotSpot() {
  if (WiFi.status() != WL_CONNECTED) {
    // Don't bother if we don't have a configuration yet
    if (!config.wifi_ssid.equals("")) {
      #ifdef SERIAL_DEBUG
      Serial.print("  - Connecting to: "); 
      Serial.println(config.wifi_ssid);
      #endif

      // Set the host name so that other devices recognise this device as the garage bot
      WiFi.hostname(config.device_name.c_str());
      WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);

      wifiEngineMode = WEM_CLIENT;
      macAddress = WiFi.macAddress();

      // Attempt to connect to the configured wifi hotspot
      WiFi.begin(config.wifi_ssid.c_str(), config.wifi_password.c_str()); 

      // Wait for the connect result
      if (WiFi.waitForConnectResult() != WL_CONNECTED) {
          #ifdef SERIAL_DEBUG
          Serial.println("  ! Failed to connect!");
          #endif
          _lastReconnectAttempt = millis();
          connected = false;
          return false;
      }       

      _handleWiFiConnected();

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
  Serial.println("  - Attempting to Broadcast dedicated Access Point...");
  #endif

  WiFi.mode(WIFI_AP_STA);
  
  if (!WiFi.softAP(AP_SSID)) {
    // The adapter was not able to set the desired SSID for the acces point
    #ifdef SERIAL_DEBUG
    Serial.println("  ! Failed to assign the softAP SSID.");
    #endif
    return false;
  }

  wifiEngineMode = WEM_AP;
  ipAddress = WiFi.softAPIP().toString();
  macAddress = WiFi.softAPmacAddress();

  #ifdef SERIAL_DEBUG
  Serial.print("  - Access Point broadcasting on SSID: '");
  Serial.print(AP_SSID);
  Serial.println("'");
  Serial.print("  - IP address: ");
  Serial.println(ipAddress);
  Serial.print("  - MAC address: ");
  Serial.println(macAddress);
  #endif

  return true;
}



/**
 * Fired whenever the device looses connection to the configured WiFi access point
 */
void WiFiEngine::_handleWiFiDisconnected() {
  connected = false;

  ipAddress = "";
  
  // Notify listeners
  if (onConnectedChanged) {
    onConnectedChanged(false);
  }
}


/**
 * Fired whenever the device connects or re-connects to the configured WiFi access point
 */
void WiFiEngine::_handleWiFiConnected() {
  connected = true;
  ipAddress = WiFi.localIP().toString();

  #ifdef SERIAL_DEBUG
  Serial.print("  - WiFi connected."); 
  Serial.print("  - IP address: "); 
  Serial.println(ipAddress);
  Serial.print("  - MAC address: ");
  Serial.println(macAddress);
  Serial.print("  - Device name: ");
  Serial.println(config.device_name);
  Serial.print("  - Web address: http://");
  Serial.print(config.mdns_name);
  Serial.println(".local");
  #endif

  // Start the mDNS service to broadcast the device's location on the network
  if(!MDNS.begin(config.mdns_name.c_str())) {
    #ifdef SERIAL_DEBUG
    Serial.println("  ! Error starting mDNS!");
    #endif
  }

  // TODO: establish if the Web Server needs to be re-created etc...

  // Notify listeners
  if (onConnectedChanged) {
    onConnectedChanged(true);
  }
}


/**
 * Initialise the routes for serving the web app content
 * 
 * TODO: Better lockdown of sending pages when in AP mode
 */
void WiFiEngine::initRoutes() {
  
  // Route for root / web page (apmode.html)
  _webServer->on("/", HTTP_GET, [&](AsyncWebServerRequest *request){
    if (wifiEngineMode == WEM_AP) {
      #ifdef SERIAL_DEBUG
      Serial.println("Sending APMODE");
      #endif
      
      request->send(LITTLEFS, "/apmode.html", String(), false, templateProcessor);
    } else {
      #ifdef SERIAL_DEBUG
      Serial.println("Sending INDEX");
      #endif
      
      request->send(LITTLEFS, "/index.html", String(), false, templateProcessor);
    }
  });

  // Route for control panel pages. These just prevent the device from 404ing when a control panel page is reloaded
  // TODO: come up with a better way of falling back onto these routes
  _webServer->on("/config", HTTP_GET, [&](AsyncWebServerRequest *request) {
    request->send(LITTLEFS, "/index.html", String(), false, templateProcessor);
  });
  _webServer->on("/calibration", HTTP_GET, [&](AsyncWebServerRequest *request) {
    request->send(LITTLEFS, "/index.html", String(), false, templateProcessor);
  });
  _webServer->on("/about", HTTP_GET, [&](AsyncWebServerRequest *request) {
    request->send(LITTLEFS, "/index.html", String(), false, templateProcessor);
  });


  // Set the wifi access point details
  _webServer->on("/setwifi", HTTP_POST, [&](AsyncWebServerRequest *request){}, NULL, [&](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    _handleSetWiFi(request, data, len);
  });

  // Set the config
  _webServer->on("/setconfig", HTTP_POST, [&](AsyncWebServerRequest *request){}, NULL, [&](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    _handleSetConfig(request, data, len);
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
      Serial.print("  - 404: ");
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
 * @see: https://github.com/me-no-dev/ESPAsyncWebServer/blob/master/examples/ESP_AsyncFSBrowser/ESP_AsyncFSBrowser.ino
 * 
 * @param client  - the websocket client
 * @param type    - the websocket event type
 * @param arg     - ??
 * @param data    - any data associated with the websocket event
 * @param len     - the length of the data
 */
void WiFiEngine::onWsEvent(AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  // Fired when a client connects to the websocket
  if (type == WS_EVT_CONNECT){
    // increment the connected socket client count
    _connectedSocketClientCount += 1;

    #ifdef SERIAL_DEBUG
    Serial.println("New incoming WebSocket connection.");
    Serial.print("Total active WebSocket connections: ");
    Serial.println(_connectedSocketClientCount);
    #endif

    // Send the current device config and status to the connected client
    sendConfigToClients(client);
    sendStatusToClients(client);
    sendSensorDataToClients(client);
  }

  // Fired when a websocket client disconnects
  else if (type == WS_EVT_DISCONNECT){
    // decrement the connected client count
    _connectedSocketClientCount -= 1;

    #ifdef SERIAL_DEBUG
    Serial.println("WebSocket connection terminated.");
    Serial.print("Total active WebSocket connetctions: ");
    Serial.println(_connectedSocketClientCount);
    #endif
  }

  // Fired when a websocket client sends data
  else if (type == WS_EVT_DATA) {
    // Pass the data parsing off to a more detailed function
    handleWebSocketData(client, arg, data, len);
  }
}


/**
 * Send the device config to connected clients.
 * Typically happens just after connection and when the config changes.
 *
 * @param client - (Optional) A specific client to send the config to
 */
void WiFiEngine::sendConfigToClients(AsyncWebSocketClient *client) {
  // Don't bother if we're not sending to a direct client and there are no active connections
  if (!client && (_connectedSocketClientCount == 0)) {
    return;
  }

  DynamicJsonDocument doc(MAX_SOCKET_SERVER_MESSAGE_SIZE);
  doc["m"] = SOCKET_SERVER_MESSAGE_CONFIG_CHANGE;
  JsonObject payload = doc.createNestedObject("p");
  payload["firmware_version"]           = FIRMWARE_VERSION;
  payload["ip_address"]                 = ipAddress;
  payload["mac_address"]                = macAddress;
  payload["mdns_name"]                  = config.mdns_name;
  payload["device_name"]                = config.device_name;
  payload["wifi_ssid"]                  = config.wifi_ssid;
  payload["mqtt_enabled"]               = config.mqtt_enabled;
  payload["mqtt_broker_address"]        = config.mqtt_broker_address;
  payload["mqtt_broker_port"]           = config.mqtt_broker_port;
  payload["mqtt_device_id"]             = config.mqtt_device_id;
  payload["mqtt_username"]              = config.mqtt_username;
  payload["mqtt_topic"]                 = config.mqtt_topic;
  payload["mqtt_state_topic"]           = config.mqtt_state_topic;
  payload["top_ir_sensor_threshold"]    = config.top_ir_sensor_threshold;
  payload["bottom_ir_sensor_threshold"] = config.bottom_ir_sensor_threshold;
  
  char json[MAX_SOCKET_SERVER_MESSAGE_SIZE];
  serializeJsonPretty(doc, json);
  
  // Send the config to a specific client
  if (client != NULL && (client->status() == WS_CONNECTED)) {
    client->text(json);
  } 

  // Send the config to all clients
  else {
    _webSocket->textAll(json);
  }
}


/**
 * Send the device status to connected clients.
 * Typically happens just after connection and when the config changes.
 *
 * @param client - (Optional) A specific client to send the config to
 */
void WiFiEngine::sendStatusToClients(AsyncWebSocketClient *client) {
  // Don't bother if we're not sending to a direct client and there are no active connections
  if (!client && (_connectedSocketClientCount == 0)) {
    return;
  }

  DynamicJsonDocument doc(MAX_SOCKET_SERVER_MESSAGE_SIZE);
  doc["m"] = SOCKET_SERVER_MESSAGE_STATUS_CHANGE;
  JsonObject payload = doc.createNestedObject("p");
  
  // Add the door status
  payload["door_state"] = doorControl.getDoorStateAsString();
  payload["mqtt_client_state"] = mqttClient.getMQTTClientStateAsString();
  payload["mqtt_client_error"] = mqttClient.getMQTTClientError();
  
  char json[MAX_SOCKET_SERVER_MESSAGE_SIZE];
  serializeJsonPretty(doc, json);
  
  // Send the config to a specific client
  if (client != NULL && (client->status() == WS_CONNECTED)) {
    client->text(json);
  } 

  // Send the config to all clients
  else {
    _webSocket->textAll(json);
  }
}


/**
 * Send the device rebooting message to connected clients
 * Happens just after the device is requested to reboot
 */
void WiFiEngine::sendRebootingToClients() {
  // Don't bother if we're not sending to a direct client and there are no active connections
  if (_connectedSocketClientCount == 0) {
    return;
  }

  DynamicJsonDocument doc(MAX_SOCKET_SERVER_MESSAGE_SIZE);
  doc["m"] = SOCKET_SERVER_MESSAGE_REBOOTING;
  JsonObject payload = doc.createNestedObject("p");
  
  char json[MAX_SOCKET_SERVER_MESSAGE_SIZE];
  serializeJsonPretty(doc, json);
  
  // Send the config to all clients
  _webSocket->textAll(json);
}


/**
 * Send the sensor data to connected clients
 * This is called on a regular basis to keep the connected clients up to date with sensor information
 *
 * @param client - (Optional) A specific client to send the config to
 */
void WiFiEngine::sendSensorDataToClients(AsyncWebSocketClient *client) {
  // Don't bother if we're not sending to a direct client and there are no active connections
  if (!client && (_connectedSocketClientCount == 0)) {
    return;
  }

  DynamicJsonDocument doc(MAX_SOCKET_SERVER_MESSAGE_SIZE);
  // Message Type
  doc["m"] = SOCKET_SERVER_MESSAGE_SENSOR_DATA;

  // Payload
  JsonObject payload = doc.createNestedObject("p");
  payload["top_detected"] = _topIRSensor->detected;
  payload["top_ambient"] = _topIRSensor->averageAmbientReading;
  payload["top_active"] = _topIRSensor->averageActiveReading;
  payload["bottom_detected"] = _bottomIRSensor->detected;
  payload["bottom_ambient"] = _bottomIRSensor->averageAmbientReading;
  payload["bottom_active"] = _bottomIRSensor->averageActiveReading;
  payload["available_memory"] = heap_caps_get_free_size(MALLOC_CAP_8BIT);

  // Send the sensor data to all connected clients
  char json[MAX_SOCKET_SERVER_MESSAGE_SIZE];
  serializeJsonPretty(doc, json);

  // Send the config to a specific client
  if (client != NULL && (client->status() == WS_CONNECTED)) {
    client->text(json);
  } 

  // Send the config to all clients
  else {
    _webSocket->textAll(json);
  }
}


/**
 * run
 *
 * Called on a regular basis by the main loop to Check the connectivity to the WiFi hotspot
 * and Send the sensor data to the connected clients.
 *
 * @param currentMillis the current milliseconds as passed down from the main loop
 */
void WiFiEngine::run (unsigned long currentMillis) {
  // If the wifiEngine is in Access Point mode, process DNS requests.
  if (wifiEngineMode == WEM_AP) {
    _dnsServer->processNextRequest();
  }

  // If the wifiEngine is in normal wifi client mode
  else {
    // evaluate the state of the WiFi connection and establish if it needs to be re-connected etc...
    if ((WiFi.status() != WL_CONNECTED) && ((currentMillis - _lastReconnectAttempt) >= RECONNECT_INTERVAL)) {
      // To our knowledge, are we supposed to be connected?!
      if (connected) {
        _handleWiFiDisconnected();
      }
      
      #ifdef SERIAL_DEBUG
      Serial.println("Reconnecting to WiFi...");
      #endif

      WiFi.disconnect();
      WiFi.reconnect();
      _lastReconnectAttempt = currentMillis;
    }

    // If the wifi is connected but we aren't aware of it, update the connected state and notify listeners
    else if ((WiFi.status() == WL_CONNECTED) && !connected) {
      _handleWiFiConnected();
    }
    
    // If the appropriate amount of time has passed and the button is stable, register the change
    if ((currentMillis - _lastSensorBroadcast) > SENSOR_BROADCAST_INTERVAL) {
      sendSensorDataToClients();
      _lastSensorBroadcast = currentMillis;
    }
  }
}

/**
 * Used when serving HTML files to replace key variables in the HTML with
 * current state data.
 * 
 * @param var - the %TEMPLATE% variable located in the html file
 */
String WiFiEngine::templateProcessor(const String& var){
  if (var == "FIRMWARE_VERSION") {
    return FIRMWARE_VERSION;
  } else if (var == "DEVICE_ADDRESS") {
    return wifiEngine.ipAddress;
  } else if (var == "DEVICE_NAME") {
    return config.device_name;
  } else {
    #ifdef SERIAL_DEBUG
    Serial.print("Unhandled HTML template variable: '");
    Serial.print(var);
    Serial.println("'");
    #endif
  }

  return var;
}


/**
 * Handles setting new WiFi connection details
 * 
 * @param request   - the incoming HTTP Post Request that triggered the action
 * @param body      - the body of the HTTP Post Request
 */
void WiFiEngine::_handleSetWiFi(AsyncWebServerRequest *request, uint8_t *data, size_t len){
  // Make sure we only capture a string of the exact message size. Also add a null terminator to the end.
  char body[len + 1];
  memcpy( body, data, len );
  body[len] = '\0';

  // Deserialize the message
  DynamicJsonDocument doc(CONFIG_FILE_MAX_SIZE);
  DeserializationError error = deserializeJson(doc, body);

  // Check for an error parsing the json
  if (error) {
    #ifdef SERIAL_DEBUG
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    #endif

    // TODO: embed the error in the JSON
    request->send(400, "text/json", "{\"success\":false}");
    return;
  }

  // Apply the new config  
  String wifiSSID = doc["wifiSSID"];
  String wifiPassword = doc["wifiPassword"];

  // Call the FileSystem method responsible for updating the WiFi config
  botFS.setWiFiSettings(wifiSSID, wifiPassword);

  // Return a 200 - Success
  request->send(200, "text/json", "{\"success\":true}");
}


/**
 * Handles setting new config details
 * 
 * @param request   - the incoming HTTP Post Request that triggered the action
 * @param body      - the body of the HTTP Post Request
 */
void WiFiEngine::_handleSetConfig(AsyncWebServerRequest *request, uint8_t *data, size_t len){
  // Make sure we only capture a string of the exact message size. Also add a null terminator to the end.
  char body[len + 1];
  memcpy( body, data, len );
  body[len] = '\0';

  // Deserialize the message
  DynamicJsonDocument doc(CONFIG_FILE_MAX_SIZE);
  DeserializationError error = deserializeJson(doc, body);

  // Check for an error parsing the json
  if (error) {
    #ifdef SERIAL_DEBUG
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    #endif

    // TODO: embed the error in the JSON
    request->send(400, "text/json", "{\"success\":false}");
    return;
  }
  
  // Extract the new config
  String mdnsName = (doc.containsKey("mdns_name") && !doc["mdns_name"].isNull() && (doc["mdns_name"].as<String>() != "")) ? doc["mdns_name"].as<String>() : DEFAULT_CONFIG_MDNS_NAME;
  String deviceName = (doc.containsKey("device_name") && !doc["device_name"].isNull() && (doc["device_name"].as<String>() != "")) ? doc["device_name"].as<String>() : DEFAULT_CONFIG_DEVICE_NAME;

  JsonVariant mqttEnabledValue = doc["mqtt_enabled"];
  bool mqttEnabled = mqttEnabledValue.isNull() ? config.mqtt_enabled : mqttEnabledValue.as<bool>();

  String mqttBrokerAddres = (doc.containsKey("mqtt_broker_address") && !doc["mqtt_broker_address"].isNull() && (doc["mqtt_broker_address"].as<String>() != "")) ? doc["mqtt_broker_address"].as<String>() : "";

  JsonVariant mqttBrokerPortValue = doc["mqtt_broker_port"];
  unsigned int mqttBrokerPort = mqttBrokerPortValue.isNull() ? DEFAULT_CONFIG_MQTT_BROKER_PORT : mqttBrokerPortValue.as<int>();

  String mqttDeviceId = (doc.containsKey("mqtt_device_id") && !doc["mqtt_device_id"].isNull() && (doc["mqtt_device_id"].as<String>() != "")) ? doc["mqtt_device_id"].as<String>() : DEFAULT_CONFIG_MQTT_DEVICE_ID;
  String mqttUsername = (doc.containsKey("mqtt_username") && !doc["mqtt_username"].isNull() && (doc["mqtt_username"].as<String>() != "")) ? doc["mqtt_username"].as<String>() : "";
  String mqttPassword = (doc.containsKey("mqtt_password") && !doc["mqtt_password"].isNull() && (doc["mqtt_password"].as<String>() != "")) ? doc["mqtt_password"].as<String>() : "";
  String mqttTopic = (doc.containsKey("mqtt_topic") && !doc["mqtt_topic"].isNull() && (doc["mqtt_topic"].as<String>() != "")) ? doc["mqtt_topic"].as<String>() : DEFAULT_CONFIG_MQTT_DEVICE_TOPIC;
  String mqttStateTopic = (doc.containsKey("mqtt_state_topic") && !doc["mqtt_state_topic"].isNull() && (doc["mqtt_state_topic"].as<String>() != "")) ? doc["mqtt_state_topic"].as<String>() : DEFAULT_CONFIG_MQTT_DEVICE_STATE_TOPIC;

  // Call the FileSystem methods responsible for updating the config
  botFS.setGeneralConfig(mdnsName, deviceName, mqttEnabled, mqttBrokerAddres, mqttBrokerPort, mqttDeviceId, mqttUsername, mqttPassword, mqttTopic, mqttStateTopic);

  // Return a 200 - Success
  request->send(200, "text/json", "{\"success\":true}");
}


/**
 * Parse and handle a websocket data message
 */
void WiFiEngine::handleWebSocketData(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    
    // We know we're only really receiving strings here so make sure the data is null-terminated
    char message[MAX_SOCKET_CLIENT_MESSAGE_SIZE];
    memcpy( message, data, len );
    message[len] = '\0';

    // First, compare the data against the "PING" string.
    if (strcmp(message, "PING") == 0) {
      // Send back a "PONG"
      client->text("PONG");
    }

    // Otherwise, attempt to parse the massage as JSON
    else {
      #ifdef SERIAL_DEBUG
      Serial.print("Socket Message Received: '");
      Serial.print(message);
      Serial.println("'");
      #endif

      DynamicJsonDocument json(MAX_SOCKET_CLIENT_MESSAGE_SIZE);
      deserializeJson(json, message);
      
      String message = json["m"].as<String>();
      JsonVariant payload = json["p"];

      // SOCKET_CLIENT_MESSAGE_BUTTON_PRESS
      if (message == SOCKET_CLIENT_MESSAGE_BUTTON_PRESS) {
        String virtualButton = payload["b"];

        #ifdef SERIAL_DEBUG
        Serial.print("SOCKET_CLIENT_MESSAGE_BUTTON_PRESS: ");
        Serial.println(virtualButton);
        #endif

        // Notify Listeners
        if (onVirtualButtonPressed) {
          onVirtualButtonPressed(toVirtualButtonType(virtualButton));
        }
      }

      // SOCKET_CLIENT_MESSAGE_REBOOT
      else if (message == SOCKET_CLIENT_MESSAGE_REBOOT) {
        reboot();
      }

      // SOCKET_CLIENT_MESSAGE_FORGET_WIFI
      else if (message == SOCKET_CLIENT_MESSAGE_FORGET_WIFI) {
        botFS.resetWiFiConfig(true);
      }

      // SOCKET_CLIENT_SET_SENSOR_THRESHOLD
      else if (message == SOCKET_CLIENT_MESSAGE_SET_SENSOR_THRESHOLD) {
        String sensorType = payload["s"];
        JsonVariant newThresholdVariant = payload["t"];
        int newThreshold = newThresholdVariant.isNull() ? 0 : newThresholdVariant.as<int>();
        // int newThreshold = payload["t"] || 0;

        if (sensorType == "TOP") {
          _topIRSensor->setThreshold(newThreshold);
          sendConfigToClients();
        } else if (sensorType == "BOTTOM") {
          _bottomIRSensor->setThreshold(newThreshold);
          sendConfigToClients();
        }
      }
    }
  }
}
