/*============================================================================*\
 * Garage Bot - remoteRepeater
 * Peter Eldred 2021-05
 * 
 * Handles trigerring the original garage door remote
\*============================================================================*/

#include "Arduino.h"
#include "_config.h"
#include "remoteRepeater.h"

/**
 * Constructor
 */
RemoteRepeater::RemoteRepeater() {}


/**
 * Initialise
 */
void RemoteRepeater::init(unsigned int pinNo){
  #ifdef SERIAL_DEBUG
  Serial.print("Initialising Garage Door Remote Repeater...");
  #endif
  
  _pinNo = pinNo;
  pinMode(_pinNo, OUTPUT);
  digitalWrite(_pinNo, LOW);

  #ifdef SERIAL_DEBUG
  Serial.println(" done.");
  #endif
}


/**
 * Run
 * 
 * @param currentMillis the current milliseconds as passed down from the main loop
 */
void RemoteRepeater::run(unsigned long currentMillis) {
  // Has the activation run its course?
  if (_activated && (currentMillis > (_startTime + REMOTE_REPEATER_DURATION_MS))) {
    _activated = false;
    _startTime = 0;
    digitalWrite(_pinNo, LOW);
    onChange(false);
  }
}


/**
 * Activate the relay that connects the contacts on the original garage remote
 */
void RemoteRepeater::activate() {
  bool oldActivated = _activated;

  _activated = true;
  _startTime = millis();

  // Notify listeners of the state change
  if (!oldActivated) {
    digitalWrite(_pinNo, HIGH);
    onChange(true);
  }
}
