/*============================================================================*\
 * Garage Bot - ledTimer
 * Peter Eldred 2021-04
 * 
 * A wrapper around the ESP32 hardware timer to simplify its usage for flashing
 * LEDs
\*============================================================================*/

#ifndef LEDTIMER_H
#define LEDTIMER_H

#include "helpers.h"

class LEDTimer {
  public:
    LEDTimer();
    
    void init();
    void run(unsigned long currentMillis);

    eventFiredFunction onTimer;
  private:
};

extern LEDTimer ledTimer;

#endif
