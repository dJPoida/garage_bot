/*============================================================================*\
 * Garage Bot - botLED
 * Peter Eldred 2021-04
 * 
 * This is a simple wrapper for an LED to simplify common functions across the 
 * app.
\*============================================================================*/

#include "botLED.h"
#include "_config.h"
#include "Arduino.h"

unsigned int flash_length =            2;
unsigned int flash_values[] =          { 1,  0 };
unsigned int flash_ticks[] =          { 4,  4 };

unsigned int flash_fast_length =       2;
unsigned int flash_fast_values[] =     { 1,  0 };
unsigned int flash_fast_ticks[] =     { 2,  2 };

unsigned int flash_pair_length =       6;
unsigned int flash_pair_values[] =     { 1,  0,  1,  0,  1,  0 };
unsigned int flash_pair_ticks[] =     { 1,  1,  1,  1,  1,  7 };

/**
 * Constructor
 */
BotLED::BotLED(unsigned int gpioNo, String name){
  _gpioNo = gpioNo;
  _name = name;  
}


/**
 * Initialise
 */
bool BotLED::init(bool state, LEDMode mode) {
  #ifdef SERIAL_DEBUG
  Serial.print("Initialising LED '");
  Serial.print(_name);
  Serial.print("'...");
  #endif

  // Set the initial state
  _mode = mode;
  _state = state;
  
  // Initialise the gpio
  pinMode(_gpioNo, OUTPUT);
  digitalWrite(_gpioNo, _state);
  
  #ifdef SERIAL_DEBUG
  Serial.println(" done.");
  #endif

  return true;
}


/**
 * Set the state of the LED (only works when the mode is LED_SOLID
 */
void BotLED::setState(bool newState) {
  if ((_mode == LED_SOLID) && (newState != _state)) {
    _state = newState;
    digitalWrite(_gpioNo, _state);
  }
}


/**
 * Return the state of the LED
 */
bool BotLED::getState() {
  return _state;
}


/**
 * Set the mode of the LED (i.e. flashing / solid etc...
 */
void BotLED::setMode(LEDMode newMode) {
  if (newMode != _mode) {
    _mode = newMode;
    _cycleNo = -1;
    _cycleTicksRemaining = 0;
    nextCycle();
  }
}


/**
 * Called by the timer to make sure that all LEDs blink in unison
 * A cycle duration is defined in the config.h
 * 
 * Only works when the mode is not LED_SOLID
 */
void BotLED::nextCycle() {
  // Should we move to the next cycle?
  if (_cycleTicksRemaining <= 0) {
    unsigned int maxCycleNo = 0;
    
    switch (_mode) {
      case LED_FLASH:
        maxCycleNo = flash_length;
        break;
      case LED_FLASH_FAST:
        maxCycleNo = flash_fast_length;
        break;
      case LED_FLASH_PAIR:
        maxCycleNo = flash_pair_length;
        break;
      default:
        break;
    }
  
    unsigned int newCycleNo = _cycleNo + 1;
    if (newCycleNo >= maxCycleNo) {
      newCycleNo = 0;
    }
    _cycleNo = newCycleNo;

    switch (_mode) {
      case LED_FLASH:
        _cycleTicksRemaining = flash_ticks[_cycleNo];
        break;
      case LED_FLASH_FAST:
        _cycleTicksRemaining = flash_fast_ticks[_cycleNo];
        break;
      case LED_FLASH_PAIR:
        _cycleTicksRemaining = flash_pair_ticks[_cycleNo];
        break;
      default:
        break;
    }

    _applyCycle();
  } else {
    _cycleTicksRemaining -= 1;
  }
}


/**
 * Evaluate the current mode and apply the current cycle number
 */
void BotLED::_applyCycle() {
  bool newState = _state;
  if (_cycleNo >= 0) {
    switch (_mode) {
      case LED_FLASH:
        newState = flash_values[_cycleNo];
        break;
      case LED_FLASH_FAST:
        newState = flash_fast_values[_cycleNo];
        break;
      case LED_FLASH_PAIR:
        newState = flash_pair_values[_cycleNo];
        break;
      default:
        break;
    }
  }

  if (newState != _state) {
    _state = newState;
    digitalWrite(_gpioNo, _state);
  }
}
