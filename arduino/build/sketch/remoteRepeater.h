/*============================================================================*\
 * Garage Bot - remoteRepeater
 * Peter Eldred 2021-05
 * 
 * Handles trigerring the original garage door remote
\*============================================================================*/

#ifndef REMOTEREPEATER_H
#define REMOTEREPEATER_H

#include "helpers.h"

class RemoteRepeater {
  public:
    RemoteRepeater();
    
    void init(unsigned int pinNo);
    void run(unsigned long currentMillis);
    void activate();

    boolValueChangedFunction onChange;
  private:
    unsigned int _pinNo;            // The pin tied to the activation relay

    bool _activated = false;        // Whether the repeater is activated or not
    unsigned long _startTime = 0;   // The time the activation began
    
};

extern RemoteRepeater remoteRepeater;

#endif
