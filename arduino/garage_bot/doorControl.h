/*============================================================================*\
 * Garage Bot - doorControl
 * Peter Eldred 2021-05
 * 
 * This class manages the state of the door and ensures that a simple request
 * to trigger the remote will not "double open" the door
\*============================================================================*/

#ifndef DOORCONTROL_H
#define DOORCONTROL_H

#include "helpers.h"

class DoorControl {
  public:
    DoorControl();
    void init();

    void setSensorStates(bool topSensor, bool bottomSensor);

    doorStateChangedFunction onStateChange;
  private:
    DoorState _doorState;
    bool _topSensor = false;      // Whether the top sensor detects the door
    bool _bottomSensor = false;   // Whether the bottom sensor detects the door
};

extern DoorControl doorControl;

#endif
