/*============================================================================*\
 * Garage Bot - rfReceiver
 * Peter Eldred 2021-04
 * 
 * Manages the RF communication from the RF remote controls
\*============================================================================*/

#ifndef RFRECEIVER_H
#define RFRECEIVER_H

#include "helpers.h"
#include <RH_ASK.h>

class RFReceiver {
  public:
    RFReceiver();
    
    bool init();
    void run(unsigned long currentMillis);

    boolValueChangedFunction onRfInputChanged;
  private:
};

extern RFReceiver rfReceiver;

#endif
