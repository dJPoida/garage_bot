/*============================================================================*\
 * Garage Bot - doorControl
 * Peter Eldred 2021-05
 * 
 * This class manages the state of the door and ensures that a simple request
 * to trigger the remote will not "double open" the door
\*============================================================================*/

#include "_config.h"
#include "helpers.h"
#include "doorControl.h"
#include "remoteRepeater.h"

/**
 * Constructor
 */
DoorControl::DoorControl(){ }


/**
 * Initialise
 */
void DoorControl::init() {
  #ifdef SERIAL_DEBUG
  Serial.print("Initialising Door Control...");
  #endif

  _doorState = DOORSTATE_UNKNOWN;

  #ifdef SERIAL_DEBUG
  Serial.println(" done.");
  #endif
}


/**
 * Basically press the garage door "activate" button
 */
void DoorControl::activate() {
  remoteRepeater.activate();

  if (_doorState == DOORSTATE_OPENING) {
    setAssumedDoorState(DOORSTATE_CLOSING);
  } else if (_doorState == DOORSTATE_OPEN) {
    setAssumedDoorState(DOORSTATE_CLOSING);
  } else if (_doorState == DOORSTATE_CLOSING) {
    setAssumedDoorState(DOORSTATE_OPENING);
  } else if (_doorState == DOORSTATE_CLOSED) {
    setAssumedDoorState(DOORSTATE_OPENING);
  } else {
    clearAssumedDoorState();
  }
}


/**
 * Open the door (if not already open / opening)
 */
void DoorControl::open() {
  if (_doorState != DOORSTATE_OPEN && _doorState != DOORSTATE_OPENING) {
    remoteRepeater.activate();
    setAssumedDoorState(DOORSTATE_OPENING);
  } else {
    #ifdef SERIAL_DEBUG
    Serial.println("Door already open / opening. Ignoring Close Command");
    #endif
  }
}


/**
 * Close the door
 */
void DoorControl::close() {
  if (_doorState != DOORSTATE_CLOSED && _doorState != DOORSTATE_CLOSING) {
    remoteRepeater.activate();
    setAssumedDoorState(DOORSTATE_CLOSING);
  } else {
    #ifdef SERIAL_DEBUG
    Serial.println("Door already closed / closing. Ignoring Close Command");
    #endif
  }
}


/**
 * When the virtual open / close buttons are pressed, we want to
 * assume that the door is in an opening / closing state until the sensors
 * can confirm or deny that state.
 */
void DoorControl::setAssumedDoorState(DoorState assumedDoorState) {
  // Only doo something if the door state has changed
  if (_doorState != assumedDoorState) {
    _doorState = assumedDoorState;
    _assumedDoorStateSetTime = millis();

    // Notify Listeners
    if (onStateChange) {
      onStateChange(_doorState);
    }
  }
}


/**
 * When the "activate" button is pressed, all bets are off.
 * We no longer know what the assumed door state is and kill any
 * expected door state off and revert to sensor detection
 */
void DoorControl::clearAssumedDoorState() {
  _assumedDoorStateSetTime = 0;

  DoorState oldDoorState = _doorState;

  _calculateDoorStateFromSensors(oldDoorState);

  // Has the door state changed?
  if (oldDoorState != _doorState) {
    if (onStateChange) {
      onStateChange(_doorState);
    }
  }
}


/**
 * Run
 * 
 * @param currentMillis the current milliseconds as passed down from the main loop
 */
void DoorControl::run(unsigned long currentMillis) {
  // Stop assuming the door state and rely on sensors
  if (_assumedDoorStateSetTime > 0 && ((_assumedDoorStateSetTime + ASSUMED_DOOR_STATE_EXPIRY) < currentMillis)) {
    clearAssumedDoorState();
  }
}


/**
 * Set the states of the two sensors used to evaluate the state of the door
 * 
 * @param {SensorDetectionState} topSensor whether the top sensor detects the door
 * @param {SensorDetectionState} bottomSensor whether the bottom sensor detects the door
 */
void DoorControl::setSensorStates(SensorDetectionState topSensor, SensorDetectionState bottomSensor) {
  // Only do something if the states have changes
  if ((topSensor != _topSensor) || (bottomSensor != _bottomSensor)) {
    _topSensor = topSensor;
    _bottomSensor = bottomSensor;

    // Not relying on assumed door state
    if (_assumedDoorStateSetTime == 0) {
      DoorState oldDoorState = _doorState;

      _calculateDoorStateFromSensors(oldDoorState);

      // Has the door state changed?
      if (oldDoorState != _doorState) {
        if (onStateChange) {
          onStateChange(_doorState);
        }
      }
    }
  }
}


/**
 * Getter for the current DoorState
 */
DoorState DoorControl::getDoorState() {
  return _doorState;
}


/**
 * Convert the current door state into a string for transport to the client
 */
String DoorControl::getDoorStateAsString() {
  switch (_doorState) {
    case DOORSTATE_OPEN:
      return "OPEN";
    case DOORSTATE_CLOSING:
      return "CLOSING";
    case DOORSTATE_CLOSED:
      return "CLOSED";
    case DOORSTATE_OPENING:
      return "OPENING";
    default: 
      return "UNKNOWN";
  }
}


/**
 * Fired when the door state changes
 */
void DoorControl::_calculateDoorStateFromSensors(DoorState oldDoorState) {
  // Door is open
  if (_topSensor == SENSOR_NOT_DETECTED && _bottomSensor == SENSOR_NOT_DETECTED) {
    _doorState = DOORSTATE_OPEN;
  }

  // Door is closed
  else if (_topSensor == SENSOR_DETECTED && _bottomSensor == SENSOR_DETECTED) {
    _doorState = DOORSTATE_CLOSED;
  }

  // Don't allow transitioning states if we don't know the previous state of the door
  else if (oldDoorState != DOORSTATE_UNKNOWN) {
    if (_doorState == DOORSTATE_OPEN) {
      _doorState = DOORSTATE_CLOSING;
    } else if (_doorState == DOORSTATE_CLOSED) {
      _doorState = DOORSTATE_OPENING;
    }
  }
}
