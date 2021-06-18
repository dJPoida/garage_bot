/*============================================================================*\
 * Garage Bot - IR Sensor
 * Peter Eldred 2021-04
 * 
 * This class represents an instance of an IR sensor for detecting whether the
 * door is closed or open at the position of the sensor.
\*============================================================================*/

#include "Arduino.h"
#include "_config.h"
#include "irsensor.h"
#include "botFS.h"

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
    _ambientReadings[i] = 0;
    _activeReadings[i] = 0;
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
    _isReading = true;
    _readingTimestamp = currentMillis;

    // Take an ambient reading and add it to the readings array
    _ambientReadings[_readingIndex] = analogRead(_pin_receiver);

    // Turn on the emitter in anticipation of the active reading
    digitalWrite(_pin_emitter, HIGH);
  } 

  // Phase 2 - After giving the emitter 2ms to emit, take a reading then turn off the emitter
  else if (_isReading && (currentMillis > (_readingTimestamp + 2))) {
    _isReading = false;
    _readingTimestamp = currentMillis;

    // Take an active reading now that the emitter is active
    _activeReadings[_readingIndex] = analogRead(_pin_receiver);
    
    // Turn off the emitter
    digitalWrite(_pin_emitter, LOW);

    // Increment the reading index for next time
    _readingIndex += 1;
    if (_readingIndex >= SENSOR_IR_SMOOTHING_READING_COUNT) {
      _readingIndex = 0;
    }
    _readingsTaken = constrain(_readingsTaken + 1, 0, (SENSOR_IR_SMOOTHING_READING_COUNT - 1));

    // If we have enough readings, evaluate the average
    if (_readingsTaken >= (SENSOR_IR_SMOOTHING_READING_COUNT - 1)) {
      bool oldDetected = detected;

      // calculate the average reading
      int sumOfActiveReadings = 0;
      int sumOfAmbientReadings = 0;
      for (int i = 0; i < SENSOR_IR_SMOOTHING_READING_COUNT; i++) {
        sumOfAmbientReadings += _ambientReadings[i];
        sumOfActiveReadings += _activeReadings[i];
      }
      averageAmbientReading = sumOfAmbientReadings / SENSOR_IR_SMOOTHING_READING_COUNT;
      averageActiveReading = sumOfActiveReadings / SENSOR_IR_SMOOTHING_READING_COUNT;

      // If the difference between the active and the ambient reading exceeds the threshold, a detection has occurred
      detected = (abs(averageAmbientReading - averageActiveReading) >= _threshold);

      // State Change in the detection - fire the on change event
      if (oldDetected != detected) {
        if (onChange) {
          onChange(detected);
        }
      } 
    }
  }
}

/**
 * Set the the detection threshold of the IR sensor
 */
void IRSensor::setThreshold(int newThreshold) {
  #ifdef SERIAL_DEBUG
  Serial.print("IRSensor '");
  Serial.print(_name);
  Serial.print("' threshold changed to: ");
  Serial.println(newThreshold);
  #endif
  
  _threshold = newThreshold;

  botFS.setIRSensorThreshold(_name, newThreshold);  
}
