/*============================================================================*\
 * Garage Bot - rfReceiver
 * Peter Eldred 2021-04
 * 
 * Manages the RF comms from the RF remote controls
\*============================================================================*/

#include "Arduino.h"
#include "RCSwitch.h"
#include "_config.h"
#include "helpers.h"
#include "rfReceiver.h"
#include "botFS.h"

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

  _mode = RF_RECEIVER_MODE_NORMAL;
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

    // When listening for registered RF Codes to activate the door
    if (_mode == RF_RECEIVER_MODE_NORMAL) {
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
    } 

    // When registering a new remote
    else if (_mode == RF_RECEIVER_MODE_REGISTERING) {
      // Only register a code if we receive it the appropriate number of times (weed out noise)
      if (receivedCode == _lastCodeReceived) {
        _receivedCodeCount += 1;
        #ifdef SERIAL_DEBUG
        Serial.print("Code '");
        Serial.print(receivedCode);
        Serial.print("' received ");
        Serial.print(_receivedCodeCount);
        Serial.print("/");
        Serial.println(REMOTE_CONSECUTIVE_CODES_FOR_REGISTRATION);
        #endif
      } else {
        _receivedCodeCount = 0;
        #ifdef SERIAL_DEBUG
        Serial.print("New code '");
        Serial.print(receivedCode);
        Serial.println("' received");
        #endif
      }
      
      // Register that the code was received for next time
      _lastCodeReceived = receivedCode;
      
      // The appropriate number of consecutive codes have been received
      if (_receivedCodeCount >= REMOTE_CONSECUTIVE_CODES_FOR_REGISTRATION) {
        // Save the new RF code into the device
        botFS.registerRFCode(receivedCode);
      }
    }

    gbSwitch.resetAvailable();
  }
}


/**
 * Set the mode that the RF Receiver is in
 * 
 * @param newMode the new RF Receiver Mode
 */
void RFReceiver::setMode(RFReceiverMode newMode) {
  RFReceiverMode oldMode = _mode;

  _mode = newMode;

  if (_mode != oldMode) {
      #ifdef SERIAL_DEBUG
      Serial.print("RF Receiver mode changed to ");
      #endif

      switch(newMode) {
        case RF_RECEIVER_MODE_NORMAL:
          #ifdef SERIAL_DEBUG
          Serial.println(" Normal");
          #endif
          break;

        case RF_RECEIVER_MODE_REGISTERING:
          #ifdef SERIAL_DEBUG
          Serial.println(" Registering new remote...");
          #endif

          // Don't allow more than 5 remotes to be registered
          if (config.stored_rf_code_count >= 5) {
            if (onError) {
              onError("The maximum of 5x remotes have already been registered. The deviec must me factory reset before more remotes can be registered.");
            }
          } else {
          }
          break;
      }

      // Notify the listeners
      if (onModeChanged) {
        onModeChanged(_mode);
      }
  }
}