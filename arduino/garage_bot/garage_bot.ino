/*============================================================================*\
 * Garage Bot
 * Author: Peter Eldred
 * Date: 2021-04
 * GitHub: TODO
 * Description: The Garage Bot repeats the RF signal of my legacy garage door
 *              remote control and exposes the state of the door as well as
 *              activation controls to a smart home via an MQTT server.
 * 
 * Hardware
 *  - ESP32 Dev Module (30 Pin)
 *    - CPU Frequency: 240Mhz
 *    - Flash Frequency: 80Mhz
 *    - Partition Scheme: Default 4MB with SPIFFS (1.2MB APP / 1.5MB SPIFFS)
 * 
 * Dependencies:
 *  - Arduino ESP32 File System Uploader (https://github.com/lorol/arduino-esp32fs-plugin)
 *  - Async TCP Library for ESP32 Arduino (https://github.com/me-no-dev/AsyncTCP)
 *  - ESP Async Web Server (https://github.com/me-no-dev/ESPAsyncWebServer)
 *  - RadioHead 433mhz Receiver (http://www.airspayce.com/mikem/arduino/RadioHead/index.html)
\*============================================================================*/


/**
 * Protection - Prevent this code from running on anything other than an ESP32
 */
#ifndef ESP32
#error This code is designed to run on ESP32 platform, not Arduino nor ESP8266! Please check your Tools->Board setting.
#endif


/**
 * Includes
 */
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "_config.h"
#include "botFS.h"
#include "botLED.h"
#include "botButton.h"
#include "ledTimer.h"
#include "wifiEngine.h"
#include "irsensor.h"
#include "rfReceiver.h"
#include "remoteRepeater.h"
#include "doorControl.h"
#include "mqttClient.h"
#include "reboot.h"


/**
 * Global Variables
 */
const char* firmwareVersion = FIRMWARE_VERSION;                           // Firmware Version
AsyncWebServer webServer(WEB_SERVER_PORT);                                // The Web Server for serving the control code
AsyncWebSocket webSocket("/ws");                                          // The Web Socker for realtime comms with the client application
DNSServer dnsServer;                                                      // A DNS Server for use when in Access Point mode
Config config;                                                            // The configuration struct for storing and reading from LITTLEFS
BotFS botFS = BotFS();                                                    // A File System Wrapper for simplifying LITTLEFS interaction
IRSensor topIRSensor = IRSensor("Top");                                   // The Top IR Sensor
IRSensor bottomIRSensor = IRSensor("Bottom");                             // The Bottom IR Sensor
BotLED powerLED = BotLED(PIN_LED_POWER, "Power");                         // The Power LED
BotLED wiFiLED = BotLED(PIN_LED_WIFI, "WiFi");                            // The WiFi LED
BotLED repeaterLED = BotLED(PIN_LED_REPEATER, "Repeater");                // The Garage Remoter Repeater Activation LED
BotLED topSensorLED = BotLED(PIN_LED_TOP_SENSOR, "Top Sensor");           // The Top Sensor LED
BotLED bottomSensorLED = BotLED(PIN_LED_BOTTOM_SENSOR, "Bottom Sensor");  // The Top Sensor LED
BotButton panelButton = BotButton(PIN_BTN_FRONT_PANEL, "Front Panel");    // The Front Panel Button
RFReceiver rfReceiver = RFReceiver();                                     // The RF Receiver for the remote controls
RemoteRepeater remoteRepeater = RemoteRepeater();                         // The object responsible for triggering the original garage remote
DoorControl doorControl = DoorControl();                                  // The object that manages the logical state of the door (open / closed / opening / closing)
LEDTimer ledTimer = LEDTimer();                                           // A Timer to help with the flashing LEDs
MQTTClient mqttClient = MQTTClient();                                     // The client which manages MQTT broadcasts and subscriptions
WiFiEngine wifiEngine = WiFiEngine();                                     // The Garage Bot's WiFi engine


/**
 * Setup
 */
void setup() {
  // Serial Initialisation
  #ifdef SERIAL_DEBUG
  Serial.begin(9600);
  Serial.println("\n==============================");
  Serial.print("\nGarage Bot v");
  Serial.println(firmwareVersion);
  Serial.println("\n==============================");
  Serial.println();
  #endif

  // Initialise the BotFS
  if (!botFS.init()) {
    // Failed to initialise the File System. Oh well. Bail.
    #ifdef SERIAL_DEBUG
    Serial.println("\n\nFAILED TO INITIALIZE THE FILE SYSTEM (LITTLEFS). HALTED!");
    #endif
    // TODO: at some point perform a "HALT" with a red-flashing light
    return;
  }

  // Initialise the WiFi Engine
  // This will automatically attempt to connect to a pre-configured
  // WiFi hotspot and if unable to do so will broadcast an Access Point
  if (!wifiEngine.init(&webServer, &webSocket, &dnsServer)) {
    // Failed to initialise the WiFi hotspot. Oh well. Bail.
    #ifdef SERIAL_DEBUG
    Serial.println("\n\nFAILED TO INITIALIZE THE WIFI ENGINE. HALTED!");
    #endif
    // TODO: at some point perform a "HALT" with a red-flashing light
    return;
  }

  // Initialise the RF receiver
  if (!rfReceiver.init()) {
    // Failed to initialise the RF Receiver. Oh well. Bail.
    #ifdef SERIAL_DEBUG
    Serial.println("\n\nFAILED TO INITIALIZE THE RF RECEIVER. HALTED!");
    #endif
    // TODO: at some point perform a "HALT" with a red-flashing light
    return;
  }

  // Sensors
  topIRSensor.init(PIN_SENSOR_TOP_EMITTER, PIN_SENSOR_TOP_RECEIVER, DEFAULT_IR_THRESHOLD);
  topIRSensor.onChange = topSensorChanged;
  bottomIRSensor.init(PIN_SENSOR_BOTTOM_EMITTER, PIN_SENSOR_BOTTOM_RECEIVER, DEFAULT_IR_THRESHOLD);
  bottomIRSensor.onChange = bottomSensorChanged;

  // Buttons
  panelButton.init();
  panelButton.onPress = panelButtonPressed;

  // LEDs
  powerLED.init(HIGH, LED_SOLID);
  wiFiLED.init(LOW, LED_SOLID);
  repeaterLED.init(LOW, LED_SOLID);
  topSensorLED.init(LOW, LED_SOLID);
  bottomSensorLED.init(LOW, LED_SOLID);

  // Flash Timer
  ledTimer.init();
  ledTimer.onTimer = updateLEDFlashes;

  // Remote Repeater
  remoteRepeater.init(PIN_REMOTE_REPEATER);
  remoteRepeater.onChange = remoteRepeaterActivationChanged;

  // Door Control
  doorControl.init();
  doorControl.onStateChange = doorControlStateChanged;

  // MQTT Client
  mqttClient.init();
  // TODO: handlers for MQTT subscriptions

  // Put the WiFi LED in flash mode if it is in Access Point mode
  if (wifiEngine.wifiEngineMode == WEM_AP) {
    wiFiLED.setMode(LED_FLASH_PAIR);
  } else {
    wiFiLED.setState(HIGH);
  }
}


/**
 * Main Loop
 */
void loop() {
  unsigned long currentMillis = millis();

  // If the wifiEngine is in Access Point mode, process DNS requests.
  if (wifiEngine.wifiEngineMode == WEM_AP) {
    dnsServer.processNextRequest();
  }

  // Run each of the delegated object controllers
  topIRSensor.run(currentMillis);
  bottomIRSensor.run(currentMillis);
  ledTimer.run(currentMillis);
  panelButton.run(currentMillis);
  rfReceiver.run(currentMillis);
  remoteRepeater.run(currentMillis);

  // Check to see if the reboot flag has been tripped
  checkReboot();
}


/**
 * Fired when the top sensor changes state
 * 
 * @param detected whether an object is being detected by the sensor
 */
void topSensorChanged(bool detected) {
  topSensorLED.setState(!detected);

  doorControl.setSensorStates(detected, bottomIRSensor.detected());

  #ifdef SERIAL_DEBUG
  Serial.print("Top: ");
  Serial.println(detected);
  #endif
}


/**
 * Fired when the bottom sensor changes state
 * 
 * @param detected whether an object is being detected by the sensor
 */
void bottomSensorChanged(bool detected) {
  bottomSensorLED.setState(!detected);

  doorControl.setSensorStates(topIRSensor.detected(), detected);

  #ifdef SERIAL_DEBUG
  Serial.print("Bottom: ");
  Serial.println(detected);
  #endif
}


/**
 * Fired when the state of the remote repeater activation changes
 * 
 * @param activated whether the remote repeater is activating or not
 */
void remoteRepeaterActivationChanged(bool activated) {
  repeaterLED.setState(activated);

  #ifdef SERIAL_DEBUG
  Serial.print("Remote Repeater Active: ");
  Serial.println(activated);
  #endif
}


/**
 * Fired when the button is pressed for one of the pre-defined durations
 * 
 * @param buttonPressType the pre-defined duration the button was pressed for
 */
void panelButtonPressed(ButtonPressType buttonPressType) {
  switch (buttonPressType) {
    case SIMPLE:
      #ifdef SERIAL_DEBUG
      Serial.println("Simple Button Press Detected");
      #endif
      remoteRepeater.activate();
      break;

    case RESET_WIFI:
      #ifdef SERIAL_DEBUG
      Serial.println("Wifi Reset Button Press Detected");
      #endif
      // Reset the WiFi config
      botFS.resetWiFiConfig();
      reboot();
      break;

    case PAIR_REMOTE:
      #ifdef SERIAL_DEBUG
      Serial.println("Pair Remote Button Press Detected");
      #endif
      powerLED.setMode(LED_FLASH_PAIR);
      break;

    default:
      break;
  }
}


/**
 * Fired when the LED Timer is triggered
 */
void updateLEDFlashes() {
  powerLED.nextCycle();
  wiFiLED.nextCycle();
}


/**
 * Fired when the Door Control state changes
 */
void doorControlStateChanged(DoorState newDoorState) {
  // Notify any connected clients of the door state change
  wifiEngine.sendStatusToClients();
  
  switch (newDoorState) {
    case DOORSTATE_OPEN:
      Serial.println("DOOR OPEN");
      break;
    case DOORSTATE_CLOSED:
      Serial.println("DOOR CLOSED");
      break;
    case DOORSTATE_OPENING:
      Serial.println("DOOR OPENING");
      break;
    case DOORSTATE_CLOSING:
      Serial.println("DOOR CLOSING");
      break;
  }
}
