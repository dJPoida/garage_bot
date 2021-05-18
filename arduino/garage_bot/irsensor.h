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
#include "helpers.h"

class IRSensor {
  public:
    IRSensor(String name);
    
    void init(unsigned int pin_emitter, unsigned int pin_receiver, int threshold);
    void run(unsigned long currentMillis);

    bool detected();                          // getter for the _detected value

    boolValueChangedFunction onChange;
  private:
    String _name;                             // The name of the Sensor (for debugging)
    unsigned long _readingTimestamp = 0;      // The timestamp of the last reading
    bool _isReading = false;                  // Whether the sensor is currently reading or not
    int _ambientReading = 0;                  // The background (ambient) reading of the IR sensor
    int _receiverReading = 0;                 // The activated reading of the IR sensor
    int _reflectedReading = 0;                // The difference between the background and activated readings
    bool _detected = false;                   // Whether the difference between background and activated constitutes a detection

    int _threshold = 0;                       // The threshold between the background and activated readings to determine a detection
    unsigned int _pin_emitter = 0;            // The PIN for the IR Emitter
    unsigned int _pin_receiver = 0;           // The PIN for the IR Receiver
};

#endif
