/*============================================================================*\
 * Garage Bot - rfReceiver
 * Peter Eldred 2021-04
 * 
 * Manages the RF communication from the RF remote controls
\*============================================================================*/

#ifndef RFRECEIVER_H
#define RFRECEIVER_H

#include "helpers.h"

class RFReceiver {
  public:
    RFReceiver();
    
    void init();
    void run(unsigned long currentMillis);

    void setMode(RFReceiverMode newMode);

    boolValueChangedFunction onButtonPress;
    receiverModeChangedFunction onModeChanged;
    errorFunction onError;

  private:
    RFReceiverMode _mode = RF_RECEIVER_MODE_NORMAL; // Whether the RF Receiver is registering a new remote or simply awaiting input
    bool _buttonPressed = false;                    // Whether the button on an RF Receiver is depressed
    unsigned long _lastButtonDown = 0;              // The last time the button was depressed (for determining when to "release" the RF button)

    void _handleButtonPressed(unsigned long currentMillis); // Fired whenever a button press is detected

    unsigned long _lastCodeReceived;                // The most recent RF Code received
    int _receivedCodeCount = 0;                     // The number of times that the most recent RF code has been received
};

extern RFReceiver rfReceiver;

#endif
