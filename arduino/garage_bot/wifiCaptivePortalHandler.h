/*============================================================================*\
 * Garage Bot - WiFiCaptivePortalHandler
 * Peter Eldred 2021-04
 * 
 * Based entirely on the ESP32 CaptivePortal Example, this code is bound to 
 * the web server when the bot is in Access Point mode and ensures any
 * connected devices can only browse to the WiFi config portal.
\*============================================================================*/

#ifndef WIFICAPTIVEPORTAL_H
#define WIFICAPTIVEPORTAL_H

#include "_config.h"

class WiFiCaptivePortalHandler : public AsyncWebHandler {
public:
  WiFiCaptivePortalHandler() {}
  virtual ~WiFiCaptivePortalHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    // Only return true if the request host is not that of the device
    return (request->host() != wifiEngine.ipAddress);
  }

  void handleRequest(AsyncWebServerRequest *request) {
    #ifdef SERIAL_DEBUG
    Serial.print("> Incoming request to: '");
    Serial.print(request->host().c_str());
    Serial.print("'. Redirecting to 'http://");
    Serial.print(wifiEngine.ipAddress);
    Serial.println("/ap-mode.html'...");
    #endif
  
    request->redirect("http://" + wifiEngine.ipAddress);
  }
};

#endif
