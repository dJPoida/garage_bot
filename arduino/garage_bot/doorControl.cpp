/*============================================================================*\
 * Garage Bot - doorControl
 * Peter Eldred 2021-05
 * 
 * This class manages the state of the door and ensures that a simple request
 * to trigger the remote will not "double open" the door
\*============================================================================*/

#include "helpers.h"
#include "doorControl.h"

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

  #ifdef SERIAL_DEBUG
  Serial.println(" done.");
  #endif
}


/**
 * Set the states of the two sensors used to evaluate the state of the door
 * 
 * @param {bool} topSensor whether the top sensor detects the door
 * @param {bool} bottomSensor whether the bottom sensor detects the door
 */
void DoorControl::setSensorStates(bool topSensor, bool bottomSensor) {
  // Only do something if the states have changes
  if ((topSensor != _topSensor) || (bottomSensor != _bottomSensor)) {
    _topSensor = topSensor;
    _bottomSensor = bottomSensor;

    DoorState oldDoorState = _doorState;

    // Door is open
    if (!topSensor && !bottomSensor) {
      _doorState = DOORSTATE_OPEN;
    }

    else if (topSensor && bottomSensor) {
      _doorState = DOORSTATE_CLOSED;
    }

    else {
      if (_doorState == DOORSTATE_OPEN) {
        _doorState = DOORSTATE_CLOSING;
      } else {
        _doorState = DOORSTATE_OPENING;
      }
    }

    // Has the door state changed?
    if (oldDoorState != _doorState) {
      onStateChange(_doorState);
    }
  }
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
