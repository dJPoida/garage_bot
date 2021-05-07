/*============================================================================*\
 * Garage Bot - rfReceiver
 * Peter Eldred 2021-04
 * 
 * Manages the RF comms from the RF remote controls
\*============================================================================*/

#include "rfReceiver.h"
#include "_config.h"
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

RH_ASK driver(2000, PIN_RF_RECEIVE, PIN_RF_DUD_TRANSMIT, 5);

/**
 * Constructor
 */
RFReceiver::RFReceiver() {}


/**
 * Initialise
 */
bool RFReceiver::init(){
  #ifdef SERIAL_DEBUG
  Serial.println("Initialising RF Remote Receiver...");
  #endif

  if (!driver.init()) return false;

  #ifdef SERIAL_DEBUG
  Serial.println("RF Remote Receiver initialised.");
  #endif

  return true;
}


/**
 * Run
 * 
 * @param currentMillis the current milliseconds as passed down from the main loop
 */
void RFReceiver::run(unsigned long currentMillis) {
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);

  if (driver.recv(buf, &buflen)) {
    #ifdef SERIAL_DEBUG
    Serial.print("Received: ");
    Serial.println((char*)buf);
    #endif
    // int i;
    // Message with a good checksum received, dump it.
    // driver.printBuffer("Got:", buf, buflen);
  }
}
