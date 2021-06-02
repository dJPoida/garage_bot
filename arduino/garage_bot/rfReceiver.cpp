/*============================================================================*\
 * Garage Bot - rfReceiver
 * Peter Eldred 2021-04
 * 
 * Manages the RF comms from the RF remote controls
\*============================================================================*/

#include "Arduino.h"
#include "rfReceiver.h"
#include "_config.h"
#include <RCSwitch.h>

RCSwitch gbSwitch = RCSwitch();

/**
 * Constructor
 */
RFReceiver::RFReceiver() {}


/**
 * Initialise
 */
void RFReceiver::init(){
  #ifdef SERIAL_DEBUG
  Serial.println("Initialising RF Remote Receiver...");
  #endif

  gbSwitch.enableReceive(digitalPinToInterrupt(PIN_RF_RECEIVE));

  #ifdef SERIAL_DEBUG
  Serial.println("RF Remote Receiver initialised.");
  #endif
}


/**
 * Run
 * 
 * @param currentMillis the current milliseconds as passed down from the main loop
 */
void RFReceiver::run(unsigned long currentMillis) {
  if (gbSwitch.available()) {
    #ifdef SERIAL_DEBUG
    Serial.println(gbSwitch.getReceivedValue());
    // TODO: This is temporary
    if (onButtonPress) {
      onButtonPress(true);
    }
//    Serial.print(" | ");
//    Serial.print((char)gbSwitch.getReceivedBitlength());
//    Serial.print(" | ");
//    Serial.print((char)gbSwitch.getReceivedDelay());
//    Serial.print(" | ");
//    Serial.print((char)gbSwitch.getReceivedRawdata());
//    Serial.print(" | ");
//    Serial.println((char)gbSwitch.getReceivedProtocol());
    #endif
    gbSwitch.resetAvailable();
  }
}
