/*============================================================================*\
 * Garage Bot - IR Sensor
 * Peter Eldred 2021-04
 * 
 * This class represents an instance of an IR sensor for detecting whether the
 * door is closed or open at the position of the sensor.
\*============================================================================*/

#include "irsensor.h"
#include "_config.h"
#include "Arduino.h"


/**
 * Constructor
 */
IRSensor::IRSensor(String name) {
  _name = name;
}


/**
 * Initialise
 * 
 * @param pin_emitter the pin which will be enabled to perform the reading
 * @param pin_receiver the pin which will receive the IR reading
 * @param threshold the difference between the ambient and 
 */
void IRSensor::init(unsigned int pin_emitter, unsigned int pin_receiver, int threshold){
  #ifdef SERIAL_DEBUG
  Serial.print("Initialising IRSensor '");
  Serial.print(_name);
  Serial.print("'...");
  #endif

  _pin_receiver = pin_receiver;
  _pin_emitter = pin_emitter;
  _threshold = threshold;
  
  pinMode(pin_receiver, INPUT);
  pinMode(pin_emitter, OUTPUT);
  digitalWrite(pin_emitter, LOW);

  #ifdef SERIAL_DEBUG
  Serial.println(" done.");
  #endif
}


/**
 * Run
 * 
 * @param currentMillis the current milliseconds as passed down from the main loop
 */
void IRSensor::run(unsigned long currentMillis) {
  // Phase 1 - Take an ambient Reading then turn on the emitter
  if (!_isReading && (currentMillis > (_readingTimestamp + SENSOR_IR_READ_DELAY - 1))) {
    _ambientReading = analogRead(_pin_receiver);
    digitalWrite(_pin_emitter, HIGH);
    _isReading = true;
    _readingTimestamp = currentMillis;
  } 

  // Phase 2 - After giving the emitter 1ms to emit, take a reading then turn off the emitter
  else if (_isReading && (currentMillis > (_readingTimestamp + 1))) {
    _receiverReading = analogRead(_pin_receiver);
    digitalWrite(_pin_emitter, LOW);
    _isReading = false;
    _readingTimestamp = currentMillis;
    _reflectedReading = abs(_ambientReading - _receiverReading);
    bool oldDetected = _detected;
    _detected = _reflectedReading >= _threshold;

    // State Change in the detection - fire the on change event
    if (oldDetected != _detected) {
      if (onChange) {
       onChange(_detected);
      }
    }
  }
}
