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
    void run(unsigned long currentMillis);

    void activate();                                        // basically press the garage door "activate" button
    void open();                                            // open the door (if not already open / opening)
    void close();                                           // close the door (if not already closed /closing)

    void setAssumedDoorState(DoorState newDoorState);
    void clearAssumedDoorState();
    void setSensorStates(SensorDetectionState topSensor, SensorDetectionState bottomSensor);
    DoorState getDoorState();
    String getDoorStateAsString();

    doorStateChangedFunction onStateChange;

  private:
    unsigned long _assumedDoorStateSetTime = 0;   // The time that an assumed door state was assigned

    DoorState _doorState;
    SensorDetectionState _topSensor = SENSOR_DETECTION_UNKNOWN;      // Whether the top sensor detects the door
    SensorDetectionState _bottomSensor = SENSOR_DETECTION_UNKNOWN;   // Whether the bottom sensor detects the door

    void _calculateDoorStateFromSensors(DoorState oldDoorState);  // Evaluate the door sensors and calculate the state
};

extern DoorControl doorControl;

#endif
