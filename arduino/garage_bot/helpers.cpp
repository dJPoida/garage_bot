/*============================================================================*\
 * Garage Bot - Helpers
 * Peter Eldred 2021-04
 * 
 * Helper functions, types and classes for the application
\*============================================================================*/

#include "Arduino.h"
#include "helpers.h"

/**
 * Determine the Mime Type of a file based on its extension
 * @param String& filename the name of the file to check
 * @return char* the calculated mime-type of the file in question
 */
String getMimeType(const String& fileName){
  // TODO: turn this into a mapped array at some point
  if (fileName.endsWith(".html")) {
    return F("text/html");
  } else if (fileName.endsWith(".css")) {
    return F("text/css");
  } else if (fileName.endsWith(".js")) {
    return F("text/javascript");
  } else if (fileName.endsWith(".svg")) {
    return F("image/svg+xml");
  }
  return F("text/plain");
}


/**
 * Convert a string representation of a virtual button type
 * to a VirtualButtonType enum value
 * 
 * @param button the string representation of the virtual button type
 */
VirtualButtonType toVirtualButtonType(const String& button){
  // TODO: turn this into a mapped array at some point
  if (button == "OPEN") {
    return OPEN;
  } else if (button == "CLOSE") {
    return CLOSE;
  }

  return ACTIVATE;
}
