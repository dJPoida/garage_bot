/*============================================================================*\
 * Garage Bot - Helpers
 * Peter Eldred 2021-04
 * 
 * Helper functions, types and classes for the application
\*============================================================================*/

#ifndef HELPERS_H
#define HELPERS_H

#include "Arduino.h"

// Used to keep track of the WiFi mode we're in
enum WiFiEngineMode {
  WEM_UNINIT,     // Uninitialised
  WEM_CLIENT,     // Connected to the Configured WiFi hotspot as a Client
  WEM_AP          // Serving a dedicated Access Point
};

// The different types of button presses that the front panel can have
enum ButtonPressType {
  SIMPLE,         // The button was pressed
  RESET_WIFI,     // The user held the button down for a factory reset duration
  PAIR_REMOTE,    // The user held the button down for a remote pairing duration
};

// Used to keep track of the mode the LED is in
enum LEDMode {
  LED_SOLID,      // Solid
  LED_FLASH,      // Simple Flash (1s on, 1s off)
  LED_FLASH_FAST, // Faster Flash (500ms on, 500ms off)
  LED_FLASH_PAIR  // Three flashes at 250ms each
};

// Used to keep track of the state of the door
enum DoorState {
  DOORSTATE_OPEN,      // Door is open
  DOORSTATE_CLOSING,   // Door is closing
  DOORSTATE_CLOSED,    // Door is closed
  DOORSTATE_OPENING    // Door is opening
};

typedef void (*boolValueChangedFunction)(bool);

typedef void (*timerFiredFunction)();

typedef void (*buttonPressedFunction)(ButtonPressType);

typedef void (*doorStateChangedFunction)(DoorState);

/**
 * Determine the Mime Type of a file based on its extension
 * @param String& filename the name of the file to check
 * @return char* the calculated mime-type of the file in question
 */
String getMimeType(const String& fileName);

#endif
