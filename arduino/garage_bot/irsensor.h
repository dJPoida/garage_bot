/*============================================================================*\
 * Garage Bot - IR Sensor
 * Peter Eldred 2021-04
 * 
 * This class represents an instance of an IR sensor for detecting whether the
 * door is closed or open at the position of the sensor.
\*============================================================================*/

#ifndef IRSENSOR_H
#define IRSENSOR_H

#include "Arduino.h"
#include "_config.h"
#include "helpers.h"

class IRSensor {
  public:
    IRSensor(String name);
    
    void init(unsigned int pin_emitter, unsigned int pin_receiver, int threshold);
    void run(unsigned long currentMillis);
    
    void setThreshold(int newThreshold);                        // Set the detection threshold

    bool detected = false;                                      // Whether the difference between ambient and activated readings constitutes a detection
    int averageAmbientReading = 0;                              // The average ambient reading
    int averageActiveReading = 0;                               // The average reading with the IR emitter activated

    boolValueChangedFunction onChange;
  private:
    String _name;                                               // The name of the Sensor (for debugging)

    int _threshold = 0;                                         // The threshold between the ambient and activated readings to determine a detection
    unsigned int _pin_emitter = 0;                              // The PIN for the IR Emitter
    unsigned int _pin_receiver = 0;                             // The PIN for the IR Receiver

    unsigned long _readingTimestamp = 0;                        // The timestamp of the last reading cycle
    bool _isReading = false;                                    // Whether the sensor is currently reading or not
    int _readingIndex = 0;                                      // The current reading index
    int _readingsTaken = 0;                                     // the number of readings take up until the max reading count to determine how many readings to sum for the average

    int _ambientReadings[SENSOR_IR_SMOOTHING_READING_COUNT];    // all of the most recent ambient readings
    int _activeReadings[SENSOR_IR_SMOOTHING_READING_COUNT];     // all of the most recent active readings
};

#endif
