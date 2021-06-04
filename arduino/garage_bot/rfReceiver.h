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

    boolValueChangedFunction onButtonPress;
  private:

};

extern RFReceiver rfReceiver;

#endif
