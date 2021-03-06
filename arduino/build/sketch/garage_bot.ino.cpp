#include <Arduino.h>
#line 1 "d:\\Development\\Arduino\\Github\\garage_bot\\arduino\\garage_bot\\garage_bot.ino"

/*============================================================================*\
 * Garage Bot
 * Author: Peter Eldred
 * Date: 2021-04
 * GitHub: https://github.com/dJPoida/garage_bot
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
 *  - RCSwitch for 433mhz Receiver (https://github.com/sui77/rc-switch)
 *  - PubSubClient for MQTT Messaging (https://github.com/knolleary/pubsubclient)
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
#include "DNSServer.h"
#include "ESPAsyncWebServer.h"
#include "AsyncTCP.h"
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
#include "otaUpdateManager.h"
#include "reboot.h"


/**
 * Global Variables
 */
const char* firmwareVersion = FIRMWARE_VERSION;                           // Firmware Version
AsyncWebServer webServer(WEB_SERVER_PORT);                                // The Web Server for serving the control code
AsyncWebSocket webSocket("/ws");                                          // The Web Socket for realtime comms with the client application
DNSServer dnsServer;                                                      // A DNS Server for use when in Access Point mode
Config config;                                                            // The configuration struct for storing and reading from LITTLEFS
BotFS botFS = BotFS();                                                    // A File System Wrapper for simplifying LITTLEFS interaction
IRSensor topIRSensor = IRSensor("TOP");                                   // The Top IR Sensor
IRSensor bottomIRSensor = IRSensor("BOTTOM");                             // The Bottom IR Sensor
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
WiFiClient espClient;                                                     // Used by the MQTT PubSubClient
PubSubClient pubSubClient = PubSubClient(espClient);                      // The MQTT PubSubClient
OTAUpdateManager otaUpdateManager = OTAUpdateManager();                   // The Over The Air (OTA) update manager

bool inError = false;                                                     // Whether the device is in an error state

/**
 * Setup
 */
#line 87 "d:\\Development\\Arduino\\Github\\garage_bot\\arduino\\garage_bot\\garage_bot.ino"
void setup();
#line 192 "d:\\Development\\Arduino\\Github\\garage_bot\\arduino\\garage_bot\\garage_bot.ino"
void loop();
#line 235 "d:\\Development\\Arduino\\Github\\garage_bot\\arduino\\garage_bot\\garage_bot.ino"
void topSensorChanged(SensorDetectionState detected);
#line 252 "d:\\Development\\Arduino\\Github\\garage_bot\\arduino\\garage_bot\\garage_bot.ino"
void bottomSensorChanged(SensorDetectionState detected);
#line 269 "d:\\Development\\Arduino\\Github\\garage_bot\\arduino\\garage_bot\\garage_bot.ino"
void remoteRepeaterActivationChanged(bool activated);
#line 284 "d:\\Development\\Arduino\\Github\\garage_bot\\arduino\\garage_bot\\garage_bot.ino"
void rfReceiverButtonPressed(bool down);
#line 308 "d:\\Development\\Arduino\\Github\\garage_bot\\arduino\\garage_bot\\garage_bot.ino"
void rfReceiverModeChanged(RFReceiverMode newMode);
#line 323 "d:\\Development\\Arduino\\Github\\garage_bot\\arduino\\garage_bot\\garage_bot.ino"
void generalErrorOccurred(String errorMessage);
#line 341 "d:\\Development\\Arduino\\Github\\garage_bot\\arduino\\garage_bot\\garage_bot.ino"
void panelButtonPressed();
#line 353 "d:\\Development\\Arduino\\Github\\garage_bot\\arduino\\garage_bot\\garage_bot.ino"
void panelButtonReleased(ButtonPressType buttonPressType);
#line 412 "d:\\Development\\Arduino\\Github\\garage_bot\\arduino\\garage_bot\\garage_bot.ino"
void updateLEDFlashes();
#line 422 "d:\\Development\\Arduino\\Github\\garage_bot\\arduino\\garage_bot\\garage_bot.ino"
void doorControlStateChanged(DoorState newDoorState);
#line 441 "d:\\Development\\Arduino\\Github\\garage_bot\\arduino\\garage_bot\\garage_bot.ino"
void handleWiFiConnectedChanged(bool newConnected);
#line 459 "d:\\Development\\Arduino\\Github\\garage_bot\\arduino\\garage_bot\\garage_bot.ino"
void handleVirtualButtonPressed(VirtualButtonType virtualButton);
#line 490 "d:\\Development\\Arduino\\Github\\garage_bot\\arduino\\garage_bot\\garage_bot.ino"
void handleMQTTStateChanged(MQTTState newState, String error);
#line 87 "d:\\Development\\Arduino\\Github\\garage_bot\\arduino\\garage_bot\\garage_bot.ino"
void setup() {
  // Serial Initialisation
  #ifdef SERIAL_DEBUG
  Serial.begin(9600);
  Serial.println("\n==============================");
  Serial.print("\nGarage Bot v");
  Serial.println(firmwareVersion);
  Serial.println("Author: Peter Eldred (aka dJPoida)");
  Serial.println("https://github.com/dJPoida/garage_bot");
  Serial.println("\n==============================");
  Serial.println();
  #endif

  // LEDs
  powerLED.init(HIGH, LED_SOLID);
  wiFiLED.init(LOW, LED_SOLID);
  repeaterLED.init(LOW, LED_SOLID);
  topSensorLED.init(LOW, LED_SOLID);
  bottomSensorLED.init(LOW, LED_SOLID);

  // Initialise the BotFS
  if (!botFS.init()) {
    // Failed to initialise the File System. Oh well. Bail.
    generalErrorOccurred("\n\nFAILED TO INITIALIZE THE FILE SYSTEM (LITTLEFS). HALTED!");

    return;
  }

  // Sensors
  topIRSensor.init(PIN_SENSOR_TOP_EMITTER, PIN_SENSOR_TOP_RECEIVER, config.top_ir_sensor_threshold);
  topIRSensor.onChange = topSensorChanged;
  bottomIRSensor.init(PIN_SENSOR_BOTTOM_EMITTER, PIN_SENSOR_BOTTOM_RECEIVER, config.bottom_ir_sensor_threshold);
  bottomIRSensor.onChange = bottomSensorChanged;

  // Initialise the WiFi Engine (if enabled)
  // This will automatically attempt to connect to a pre-configured
  // WiFi hotspot and if unable to do so will broadcast an Access Point
  if (config.wifi_enabled && !wifiEngine.init(&webServer, &webSocket, &dnsServer, &topIRSensor, &bottomIRSensor)) {
    // Failed to initialise the WiFi hotspot. Oh well. Bail.
    generalErrorOccurred("\n\nFAILED TO INITIALIZE THE WIFI ENGINE. HALTED!");
    return;
  }

  // Buttons
  panelButton.init();
  panelButton.onPress = panelButtonPressed;
  panelButton.onReleased = panelButtonReleased;

  // Flash Timer
  ledTimer.init();
  ledTimer.onTimer = updateLEDFlashes;

  // RF receiver
  rfReceiver.init();
  rfReceiver.onButtonPress = rfReceiverButtonPressed;
  rfReceiver.onModeChanged = rfReceiverModeChanged;
  rfReceiver.onError = generalErrorOccurred;

  // Remote Repeater
  remoteRepeater.init(PIN_REMOTE_REPEATER);
  remoteRepeater.onChange = remoteRepeaterActivationChanged;

  // Door Control
  doorControl.init();
  doorControl.onStateChange = doorControlStateChanged;

  if (config.wifi_enabled) {
    // If the wifi engine is in access point mode
    if (wifiEngine.wifiEngineMode == WEM_AP) {
      // Put the WiFi LED in flash mode if it is in Access Point mode
      wiFiLED.setMode(LED_FLASH_REGISTER);
    } 
    
    // IF the wifi engine is in regular mode
    else {
      // Initialise the OTA manager
      otaUpdateManager.init();

      // Initialise the MQTT Client
      if (config.mqtt_enabled) {
        mqttClient.init(&pubSubClient);
        mqttClient.onStateChange = handleMQTTStateChanged;
      }

      // Listen to changes in the WiFi client's connectivity
      wifiEngine.onConnectedChanged = handleWiFiConnectedChanged;
      wifiEngine.onVirtualButtonPressed = handleVirtualButtonPressed;
      handleWiFiConnectedChanged(wifiEngine.connected);

      // Allow incoming websocket connections
      wifiEngine.allowIncomingWebSockets();
    }
  }

  // All done
  #ifdef SERIAL_DEBUG
  Serial.println("\nInitialisation Complete.\n==============================");
  Serial.println();
  #endif
}


/**
 * Main Loop
 */
void loop() {
  if (!inError) {
    unsigned long currentMillis = millis();

    // Run each of the delegated object controllers
    if (!config.updating_config) {
      topIRSensor.run(currentMillis);
      bottomIRSensor.run(currentMillis);
      ledTimer.run(currentMillis);
      panelButton.run(currentMillis);
      rfReceiver.run(currentMillis);
      remoteRepeater.run(currentMillis);
      doorControl.run(currentMillis);
    }
    
    // Wifi functions
    if (config.wifi_enabled) {
      // The OTAUpdateManager processes requests to update the software
      if (!config.updating_config) {
        otaUpdateManager.run(currentMillis);
      }

      // The wifi engine.run will process Access Point requests and check and manage for wifi disconnections
      // This also sends sensor data to any connected socket clients
      wifiEngine.run(currentMillis);
    
      // Only run the MQTT loop if the wifi and mqtt services are enabled
      if (!config.updating_config && config.mqtt_enabled) {
        mqttClient.run(currentMillis);
      }
    }

    // Check to see if the reboot flag has been tripped
    checkReboot();
  }
}


/**
 * Fired when the top sensor changes state
 * 
 * @param detected whether an object is being detected by the sensor
 */
void topSensorChanged(SensorDetectionState detected) {
  topSensorLED.setState(detected == SENSOR_NOT_DETECTED);

  doorControl.setSensorStates(detected, bottomIRSensor.detected);

  #ifdef SERIAL_DEBUG
  Serial.print("Top: ");
  Serial.println(detected == SENSOR_DETECTED ? "Detected" : "Not Detected");
  #endif
}


/**
 * Fired when the bottom sensor changes state
 * 
 * @param detected whether an object is being detected by the sensor
 */
void bottomSensorChanged(SensorDetectionState detected) {
  bottomSensorLED.setState(detected == SENSOR_NOT_DETECTED);

  doorControl.setSensorStates(topIRSensor.detected, detected);

  #ifdef SERIAL_DEBUG
  Serial.print("Bottom: ");
  Serial.println(detected == SENSOR_DETECTED ? "Detected" : "Not Detected");
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
 * Fired when the RF Receiver detects a button press
 * 
 * @param down whether the remote button is being "pressed"
 */
void rfReceiverButtonPressed(bool down) {
  #ifdef SERIAL_DEBUG
  Serial.print("Remote Button ");
  #endif
  
  if (down) {
    #ifdef SERIAL_DEBUG
    Serial.println("Pressed");
    #endif

    // Activate the garage door control
    doorControl.activate();
  } else {
    #ifdef SERIAL_DEBUG
    Serial.println("Released");
    #endif
  }
}


/**
 * Fired when the RF Receiver changes mode.
 * Only really used to change how the RF Receiver LED flashes
 */
void rfReceiverModeChanged(RFReceiverMode newMode) {
  switch (newMode) {
    case RF_RECEIVER_MODE_REGISTERING:
      repeaterLED.setMode(LED_FLASH_REGISTER);
      break;
    default:
      repeaterLED.setMode(LED_SOLID);
      break;
  }
}


/**
 * Fired whenever an error has occurred for some reason
 */
void generalErrorOccurred(String errorMessage) {
  inError = true;

  powerLED.set(true, LED_FLASH);
  wiFiLED.set(true, LED_FLASH);
  repeaterLED.set(true, LED_FLASH);

  #ifdef SERIAL_DEBUG
  Serial.println("\nGeneral Error Occurred!");
  Serial.println(errorMessage);
  Serial.println("Device Halted");
  #endif
}


/**
 * Fired when the button is pressed
 */
void panelButtonPressed() {
  // To help the user count the number of seconds passed, the Power LED will flash 500ms ON / 500ms OFF
  // for as long as they are holding the button down.
  powerLED.set(true, LED_FLASH);
}


/**
 * Fired when the button is released after one of the pre-defined durations
 * 
 * @param buttonPressType the pre-defined duration the button was pressed for
 */
void panelButtonReleased(ButtonPressType buttonPressType) {
  // The Power LED would have been blinking up until this point. Reset it to a solid ON.
  powerLED.set(true, LED_SOLID);

  switch (buttonPressType) {
    case SIMPLE:
      #ifdef SERIAL_DEBUG
      Serial.println("Simple Button Press Detected");
      #endif
      doorControl.activate();
      break;

    case REGISTER_REMOTE:
      #ifdef SERIAL_DEBUG
      Serial.println("Register Remote Button Press Detected");
      #endif
      rfReceiver.setMode(RF_RECEIVER_MODE_REGISTERING);
      break;

    case RESET_WIFI:
      #ifdef SERIAL_DEBUG
      Serial.println("Wifi Reset Button Press Detected");
      #endif
      // Reset the WiFi config
      botFS.resetWiFiConfig(true);
      break;

    case DISABLE_WIFI:
      #ifdef SERIAL_DEBUG
      Serial.println("Disable Wifi Button Press Detected");
      #endif
      // Disable the WiFi
      botFS.resetWiFiConfig(false);
      break;

    case FACTORY_RESET:
      #ifdef SERIAL_DEBUG
      Serial.println("Factory Reset Button Press Detected");
      #endif
      // Full Factory Reset
      // Turn on ALL the LEDs
      powerLED.set(true);
      wiFiLED.set(true);
      repeaterLED.set(true);
      topSensorLED.set(true);
      bottomSensorLED.set(true);
      delay(3000);
      botFS.factoryReset();
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
  repeaterLED.nextCycle();
  wiFiLED.nextCycle();
}


/**
 * Fired when the Door Control state changes
 */
void doorControlStateChanged(DoorState newDoorState) {
  // Notify any connected clients of the door state change
  if (config.wifi_enabled) {
    wifiEngine.sendStatusToClients();

    if (config.mqtt_enabled) {
      mqttClient.sendDoorStateToBroker();
    }
  }

  #ifdef SERIAL_DEBUG
  Serial.println(doorControl.getDoorStateAsString());
  #endif
}


/**
 * Fired by the WiFi engine when the connected boolean changes
 */
void handleWiFiConnectedChanged(bool newConnected) {
  // The WiFi engine is now connected to the configured hotspot
  if (newConnected) {
    // Display a solid blue WiFi LED
    wiFiLED.set(true, LED_SOLID);
  } 
  
  // The WiFi engine has lost connection to the configured hotspot
  else {
    // Flash the blue WiFi LED.
    wiFiLED.set(false, LED_FLASH);
  }
}


/**
 * Fired when a virtual button is pressed
 */
void handleVirtualButtonPressed(VirtualButtonType virtualButton) {
  DoorState currentDoorState = doorControl.getDoorState();

  switch (virtualButton) {
    case ACTIVATE:
      #ifdef SERIAL_DEBUG
      Serial.println("Virtual Activate Button Press Detected");
      #endif
      doorControl.activate();
      break;

    case OPEN:
      #ifdef SERIAL_DEBUG
      Serial.println("Virtual Open Button Press Detected");
      #endif
      doorControl.open();
      break;

    case CLOSE:
      #ifdef SERIAL_DEBUG
      Serial.println("Virtual Close Button Press Detected");
      #endif
      doorControl.close();
      break;
  }
}


/**
 * Fired by the MQTT Client when its state changes
 */
void handleMQTTStateChanged(MQTTState newState, String error) {
  #ifdef SERIAL_DEBUG
  Serial.print("MQTT State Changed: ");
  Serial.println(mqttClient.getMQTTStateAsString());
  #endif

  // Notify any connected clients of the MQTT Client state change
  if (config.wifi_enabled) {
    wifiEngine.sendStatusToClients();
  }
}

