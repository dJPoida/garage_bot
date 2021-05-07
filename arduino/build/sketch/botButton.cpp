/*============================================================================*\
 * Garage Bot - botButton
 * Peter Eldred 2021-04
 * 
 * This is a simple wrapper for a button to simplify common functions like
 * timed presses and debouncing across the app.
\*============================================================================*/

#include "Arduino.h"
#include "botButton.h"
#include "_config.h"


/**
 * Constructor
 */
BotButton::BotButton(unsigned int gpioNo, String name){
  _gpioNo = gpioNo;
  _name = name;  
}


/**
 * Initialise
 */
bool BotButton::init() {
  #ifdef SERIAL_DEBUG
  Serial.print("Initialising Button '");
  Serial.print(_name);
  Serial.print("'...");
  #endif

  // Initialise the gpio
  pinMode(_gpioNo, INPUT);

  #ifdef SERIAL_DEBUG
  Serial.println(" done.");
  #endif

  return true;
}


/**
 * Run
 * 
 * @param currentMillis the current milliseconds as passed down from the main loop
 */
void BotButton::run(unsigned long currentMillis) {
  bool reading = digitalRead(_gpioNo);

  // If the button reading has changed
  if (reading != _lastState) {
    // Make note of the change as a debounce timestamp
    _lastDebounceTime = currentMillis;
  }
    
  // If the appropriate amount of time has passed and the button is stable, register the change
  if ((currentMillis - _lastDebounceTime) > BTN_DEBOUNCE_DELAY) {
    // if the button state has changed:
    if (reading != _state) {
      // Register the button state change (and for how long the button has been in its previous state)
      _handleStateChange(reading, currentMillis, reading ? 0 : (currentMillis - _downStartTime));
    }
  }

  // Keep track of the reading for the next time around
  _lastState = reading;
}


/**
 * Fired when one of the button presses has passed the de-bounce check
 *
 * @param newState      - whether the button is depressed or released
 * @param currentMillis - the current milliseconds as passed down from the main loop
 * @param duration      - for how long the button was in the previous state
 */
void BotButton::_handleStateChange(bool newState, unsigned long currentMillis, unsigned long duration) {
  _state = newState;

  #ifdef SERIAL_DEBUG
  Serial.print("Button: '");
  Serial.print(_name);
  Serial.print("' ");
  Serial.print(newState ? "DOWN" : "UP");
  #endif

  // Button is Down
  if (newState) {
    #ifdef SERIAL_DEBUG
    Serial.println("");
    #endif

    _downStartTime = currentMillis;
  }

  // Button is Up
  else {
    #ifdef SERIAL_DEBUG
    Serial.print(" ");
    Serial.print(duration);
    Serial.println("ms");
    #endif

    _downStartTime = 0;

    // Simple Press
    if (onPress) {
      if (duration < BTN_PRESS_DURATION) {
        onPress(SIMPLE);
      } 
      
      // Pair Remote
      else if (duration > BTN_PAIR_REMOTE_DURATION) {
        onPress(PAIR_REMOTE);
      }
      
      // Factory Reset
      else if (duration > BTN_RESET_WIFI_DURATION) {
        onPress(RESET_WIFI);
      }

    }
  }
}
