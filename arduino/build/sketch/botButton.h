/*============================================================================*\
 * Garage Bot - botButton
 * Peter Eldred 2021-04
 * 
 * This is a simple wrapper for a button to simplify common functions like
 * timed presses and debouncing across the app.
\*============================================================================*/

#ifndef BOTBUTTON_H
#define BOTBUTTON_H

#include "Arduino.h"
#include "helpers.h"

class BotButton {
  public:
    BotButton(unsigned int gpioNo, String name);
    bool init();
    void run(unsigned long currentMillis);

    eventFiredFunction onPress;
    buttonPressedFunction onReleased;
  private:
    String _name;                           // The name of the LED (for debugging)
    unsigned int _gpioNo;                   // The GPIO the led is connected to

    bool _state = LOW;                      // The current state of the button
    bool _lastState = LOW;                  // The previous state of the button when it last left the run() worker
    unsigned long _lastDebounceTime = 0;    // The currentMillis of the button when it was first triggered (to prevent phantom presses)
    unsigned long _downStartTime = 0;       // The time the button was depressed

    // A method to be called internally when the button changes state.
    void _handleStateChange(bool newState, unsigned long currentMillis, unsigned long duration);
};

#endif
