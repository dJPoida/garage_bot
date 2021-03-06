/*============================================================================*\
 * Garage Bot - Helpers
 * Peter Eldred 2021-04
 * 
 * Helper functions, types and classes for the application
\*============================================================================*/

#ifndef HELPERS_H
#define HELPERS_H

#include "Arduino.h";

// Used to keep track of the WiFi mode we're in
enum WiFiEngineMode {
  WEM_UNINIT,     // Uninitialized
  WEM_CLIENT,     // Connected to the Configured WiFi hotspot as a Client
  WEM_AP          // Serving a dedicated Access Point
};

// The different types of button presses that the front panel can have
enum ButtonPressType {
  SIMPLE,           // The button was pressed
  REGISTER_REMOTE,  // The user held the button down for a remote registration duration
  RESET_WIFI,       // The user held the button down for a wifi reset duration
  DISABLE_WIFI,     // The user wants to wifi disable duration
  FACTORY_RESET,    // The user held the button down for a factory reset duration
};

// The different types of virtual buttons that can be "pressed" by connected devices and API calls
enum VirtualButtonType {
  ACTIVATE,       // Activate the Remote Repeater (don't care about open/close)
  OPEN,           // Open the door
  CLOSE,          // Close the door
};

// Used to keep track of the mode the LED is in
enum LEDMode {
  LED_SOLID,      // Solid
  LED_FLASH,      // Simple Flash (1s on, 1s off)
  LED_FLASH_FAST, // Faster Flash (500ms on, 500ms off)
  LED_FLASH_REGISTER  // Three flashes at 250ms each
};

// Used to keep track of the state of the door
// These values need to match the `door-state.const.ts` constants in the client app
enum DoorState {
  DOORSTATE_UNKNOWN,   // Door state is unknown
  DOORSTATE_OPEN,      // Door is open
  DOORSTATE_CLOSING,   // Door is closing
  DOORSTATE_CLOSED,    // Door is closed
  DOORSTATE_OPENING    // Door is opening
};

// Used to keep track of the mode of the rf receiver
enum RFReceiverMode {
  RF_RECEIVER_MODE_NORMAL,       // Operating as normal, waiting for incoming signals
  RF_RECEIVER_MODE_REGISTERING,  // Waiting for new remotes to be registered
};

// Used to determine the state of the sensors
enum SensorDetectionState {
  SENSOR_DETECTION_UNKNOWN, // Unknown / no detection data yet
  SENSOR_NOT_DETECTED,      // No detection       
  SENSOR_DETECTED,          // Detection
};

// Wrapper around the MQTT PubSubClient state values
enum MQTTState {
  MQTT_STATE_CONNECTION_TIMEOUT,
  MQTT_STATE_CONNECTION_LOST,
  MQTT_STATE_CONNECT_FAILED,
  MQTT_STATE_DISCONNECTED,
  MQTT_STATE_CONNECTED,
  MQTT_STATE_CONNECT_BAD_PROTOCOL,
  MQTT_STATE_CONNECT_BAD_CLIENT_ID,
  MQTT_STATE_CONNECT_UNAVAILABLE,
  MQTT_STATE_CONNECT_BAD_CREDENTIALS,
  MQTT_STATE_CONNECT_UNAUTHORIZED,

  // This one is ours and is used to depict an invalid MQTT Configuration
  MQTT_STATE_DISABLED,
  MQTT_STATE_CONFIG_ERROR,
};

typedef void (*eventFiredFunction)();

typedef void (*boolValueChangedFunction)(bool);

typedef void (*buttonPressedFunction)(ButtonPressType);

typedef void (*doorStateChangedFunction)(DoorState);

typedef void (*mqttStateChangedFunction)(MQTTState, String);

typedef void (*receiverModeChangedFunction)(RFReceiverMode);

typedef void (*errorFunction)(String);

typedef void (*virtualButtonPressedFunction)(VirtualButtonType);

typedef void (*sensorDetectionStateChangedFunction)(SensorDetectionState);

/**
 * Determine the Mime Type of a file based on its extension
 * @param String& filename the name of the file to check
 * @return char* the calculated mime-type of the file in question
 */
String getMimeType(const String& fileName);

/**
 * Convert a string representation of a virtual button type
 * to a VirtualButtonType enum value
 */
VirtualButtonType toVirtualButtonType(const String& button);

#endif
