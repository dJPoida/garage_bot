/*============================================================================*\
 * Garage Bot - botLED
 * Peter Eldred 2021-04
 * 
 * This is a simple wrapper for an LED to simplify common functions across the 
 * app.
\*============================================================================*/

#ifndef BOTLED_H
#define BOTLED_H

#include "Arduino.h"
#include "helpers.h"

class BotLED{
  public:
    BotLED(unsigned int gpioNo, String name);
    bool init(bool state = LOW, LEDMode mode = LED_SOLID);

    void setMode(LEDMode newMode);
    void setState(bool newState);
    void set(bool newState, LEDMode newMode = LED_SOLID);
    void nextCycle();

    bool getState();
  private:
    String _name;                           // The name of the LED (for debugging)
    unsigned int _gpioNo;                   // The GPIO the led is connected to

    int _cycleNo = 0;                       // The current cycle number (for flashing etc...)
    unsigned int _cycleTicksRemaining = 0;  // The number of ticks remaining for the current cycleNo
    LEDMode _mode = LED_SOLID;              // The mode of the LED (flashing / solid etc...)
    bool _state = LOW;                      // The state of the LED

    void _applyCycle();                      // Apply the current cycleNo to the LED State
};

#endif
