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

  // initialize all the readings to 0:
  for (int i = 0; i < SENSOR_IR_SMOOTHING_READING_COUNT; i++) {
    _readings[i] = 0;
  }
  
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
  if (!_isReading && (currentMillis > (_readingTimestamp + SENSOR_IR_READ_DELAY - 2))) {
    _ambientReading = analogRead(_pin_receiver);
    digitalWrite(_pin_emitter, HIGH);
    _isReading = true;
    _readingTimestamp = currentMillis;
  } 

  // Phase 2 - After giving the emitter 2ms to emit, take a reading then turn off the emitter
  else if (_isReading && (currentMillis > (_readingTimestamp + 2))) {
    _receiverReading = analogRead(_pin_receiver);
    _isReading = false;
    _readingTimestamp = currentMillis;
    int reflectedReading = abs(_ambientReading - _receiverReading);

    // Turn off the emitter
    digitalWrite(_pin_emitter, LOW);

    // Add the reflected reading to the reading array
    _readings[_readingIndex] = reflectedReading;

    // Increment the reading index for next time
    _readingIndex += 1;
    if (_readingIndex > SENSOR_IR_SMOOTHING_READING_COUNT) {
      _readingIndex = 0;
    }
    _readingsTaken += 1;
    _readingsTaken = constrain(_readingsTaken, 0, SENSOR_IR_SMOOTHING_READING_COUNT);

    // If we have enough readings, evaluate the average
    if (_readingsTaken >= SENSOR_IR_SMOOTHING_READING_COUNT) {
      bool oldDetected = _detected;

      // calculate the average reading
      int sumOfReadings = 0;
      for (int i = 0; i < SENSOR_IR_SMOOTHING_READING_COUNT; i++) {
        sumOfReadings += _readings[i];
      }
      int averageReading = sumOfReadings / SENSOR_IR_SMOOTHING_READING_COUNT;
      
      _detected = averageReading >= _threshold;

      // State Change in the detection - fire the on change event
      if (oldDetected != _detected) {
        if (onChange) {
          onChange(_detected);
        }
      } 
    }
  }
}


/**
 * Getter for the _detected property
 */
bool IRSensor::detected() {
  return _detected;
}
