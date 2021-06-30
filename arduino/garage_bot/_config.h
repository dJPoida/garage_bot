/*============================================================================*\
 * Garage Bot - Config
 * Peter Eldred 2021-04
 * 
 * All of the system config goes here
\*============================================================================*/
#ifndef CONFIG_H
#define CONFIG_H

#include "Arduino.h"

#define FIRMWARE_VERSION "0.0.1"

// Comment out this line to remove serial debug from all files
#define SERIAL_DEBUG

// The five LEDs on the front panel
#define PIN_LED_POWER 21
#define PIN_LED_WIFI 19
#define PIN_LED_REPEATER 4
#define PIN_LED_TOP_SENSOR 5
#define PIN_LED_BOTTOM_SENSOR 18

// The button on the front panel
#define PIN_BTN_FRONT_PANEL 15

// The RF Data Receive Pin
#define PIN_RF_RECEIVE 27

// The Remote Repeater Relay Activation Pin
#define PIN_REMOTE_REPEATER 26

// A delay (ms) to wait to prevent phantom button presses
#define BTN_DEBOUNCE_DELAY 25      

// Anything below this duration is considered a "simple" activate door press
#define BTN_ACTIVATE_DOOR_DURATION 1000

// Anything above this duration is considered a "Register an RF Remote" request
#define BTN_REGISTER_REMOTE_DURATION 3000

// Anything above this duration is considered a "Reset Wifi" request
#define BTN_RESET_WIFI_DURATION 7000

// Anything above this duration is considered a "Disable Wifi" request
#define BTN_DISABLE_WIFI_DURATION 10000

// Anything above this duration is considered a "Factory Reset"
#define BTN_FACTORY_RESET_DURATION 15000

// The garage door sensor pins
#define PIN_SENSOR_TOP_EMITTER 32
#define PIN_SENSOR_TOP_RECEIVER 34
#define PIN_SENSOR_BOTTOM_EMITTER 33
#define PIN_SENSOR_BOTTOM_RECEIVER 35

// The time between reads of the IR Sensors (ms)
#define SENSOR_IR_READ_DELAY 100

// The number of readings to use to average out the value
#define SENSOR_IR_SMOOTHING_READING_COUNT 20

// The number of milliseconds to wait in between sensor data broadcast to the connected socket clients
#define SENSOR_BROADCAST_INTERVAL 1000

// The SSID and network domain name when the device is running in Access Point mode
#define AP_SSID "garagebot"

// The port that the web server is served on.
#define WEB_SERVER_PORT 80          

// How often the WiFi and MQTT clients should attempt to re-connect when disconnected
#define RECONNECT_INTERVAL 30000

// How many milliseconds for each interval/tick in the LED timer
#define LED_TIMER_CYCLE_MS 125

// How many milliseconds the remote repeater should "hold down the garage remote button" for
#define REMOTE_REPEATER_DURATION_MS 1000

// How many consecutive successful RF receipts are required when registering a new remote
#define REMOTE_CONSECUTIVE_CODES_FOR_REGISTRATION 10

// The maximum number of bytes we can expect to send to the client
#define MAX_SOCKET_SERVER_MESSAGE_SIZE 1024

// The maximum number of bytes we can expect to received from the client
#define MAX_SOCKET_CLIENT_MESSAGE_SIZE 256

// The maximum number of concurrent socket connections to accept
#define MAX_SOCKET_CONNECTIONS 10

// The maximum size of the config file in bytes
#define CONFIG_FILE_MAX_SIZE 1024

// When assuming a door state - ignore sensors for this duration
#define ASSUMED_DOOR_STATE_EXPIRY 5000

// Defaults for some config values
#define DEFAULT_IR_THRESHOLD 150
#define DEFAULT_CONFIG_MDNS_NAME "garagebot"
#define DEFAULT_CONFIG_DEVICE_NAME "GarageBot"
#define DEFAULT_CONFIG_MQTT_BROKER_PORT 1883
#define DEFAULT_CONFIG_MQTT_DEVICE_ID "Garage_Bot"
#define DEFAULT_CONFIG_MQTT_DEVICE_TOPIC "garage/door"
#define DEFAULT_CONFIG_MQTT_DEVICE_STATE_TOPIC "garage/door/state"

/**
 * Config struct for storing and loading data from the SPIFFS partition
 */
struct Config {
  // The name to use in the mdns address that clients can use to connect to the device without the IP (i.e. http://garagebot.local)
  String mdns_name                          = DEFAULT_CONFIG_MDNS_NAME;

  // The device name to display to other devices on the network and in the app
  String device_name                        = DEFAULT_CONFIG_DEVICE_NAME;
  
  // Whether WiFi is enabled
  bool wifi_enabled                         = false;
  
  // The SSID of the wifi network that the garage bot is configured to connect to
  String wifi_ssid                          = "";
  
  // The Password of the wifi network that the garage bot is configured to connect to
  String wifi_password                      = "";
  
  // Whether the device should attempt to integrate with an MQTT broker
  bool mqtt_enabled                         = false;
  
  // The IP address of the MQTT Broker
  String mqtt_broker_address                = "";

  // The MQTT Broker Port Number
  unsigned int mqtt_broker_port             = DEFAULT_CONFIG_MQTT_BROKER_PORT;

  // The Device ID to use when connecting to the MQTT Broker
  String mqtt_device_id                     = DEFAULT_CONFIG_MQTT_DEVICE_ID;
  
  // The username when connecting to the MQTT broker
  String mqtt_username                      = "";

  // The password when connecting to the MQTT broker
  String mqtt_password                      = "";
  
  // The MQTT topic used for communicating instructions (open / close etc)
  String mqtt_topic                         = DEFAULT_CONFIG_MQTT_DEVICE_TOPIC;
  
  // The MQTT topic used for communicating the state of the door (opened / closed / etc)
  String mqtt_state_topic                   = DEFAULT_CONFIG_MQTT_DEVICE_STATE_TOPIC;
  
  // The number of registered RF remote codes (5 Max)
  byte stored_rf_code_count                 = 0;
  
  // 5 RF codes can be stored
  unsigned long rf_codes[5]                 = {0, 0, 0, 0, 0};
  
  // The threshold for detection for the Top IR Sensor  
  unsigned int top_ir_sensor_threshold      = DEFAULT_IR_THRESHOLD;

  // The threshold for detection for the Bottom IR Sensor  
  unsigned int bottom_ir_sensor_threshold   = DEFAULT_IR_THRESHOLD;
};

extern Config config;

/**
 * WebSocket message definitions for sending / receiving data from connected clients
 * These need to be kept in sync with the `socket-server-message.const.ts` and  
 * `socket-client-message.const.ts` in the `app` website code
 */
#define SOCKET_CLIENT_MESSAGE_BUTTON_PRESS "BP"
#define SOCKET_CLIENT_MESSAGE_SET_SENSOR_THRESHOLD "ST"
#define SOCKET_CLIENT_MESSAGE_REBOOT "RB"
#define SOCKET_CLIENT_MESSAGE_FORGET_WIFI "FW"
#define SOCKET_CLIENT_MESSAGE_RESET_TO_FACTORY_DEFAULTS "RF"
#define SOCKET_SERVER_MESSAGE_STATUS_CHANGE "SC"
#define SOCKET_SERVER_MESSAGE_CONFIG_CHANGE "CC"
#define SOCKET_SERVER_MESSAGE_SENSOR_DATA "SD"
#define SOCKET_SERVER_MESSAGE_REBOOTING "RB"


#endif
