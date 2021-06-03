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
    int receivedCode = gbSwitch.getReceivedValue();
    byte matchingCode = 0;

    // TODO: We may need these later to better verify the remote
    // Serial.print((char)gbSwitch.getReceivedBitlength());
    // Serial.print((char)gbSwitch.getReceivedDelay());
    // Serial.print((char)gbSwitch.getReceivedRawdata());
    // Serial.println((char)gbSwitch.getReceivedProtocol());
    
    // Iterate over the registered codes and check the incoming code
    for (byte i = 1; i <= 5; i++){
      if (config.rf_codes[i - 1] == receivedCode) {
        matchingCode = i;
      }
    }

    if (matchingCode > 0) {
      #ifdef SERIAL_DEBUG
      Serial.print("Registered code ");
      Serial.print(matchingCode);
      Serial.print(": '");
      Serial.print(receivedCode);
      Serial.println("' received");
      #endif

      if (onButtonPress) {
        onButtonPress(true);
      }
    } else {
      #ifdef SERIAL_DEBUG
      Serial.print("Unregistered code '");
      Serial.print(receivedCode);
      Serial.println("' received");
      #endif
    }
    gbSwitch.resetAvailable();
  }
}
