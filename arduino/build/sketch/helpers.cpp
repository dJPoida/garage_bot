/*============================================================================*\
 * Garage Bot - Helpers
 * Peter Eldred 2021-04
 * 
 * Helper functions, types and classes for the application
\*============================================================================*/

#include "Arduino.h"

/**
 * Determine the Mime Type of a file based on its extension
 * @param String& filename the name of the file to check
 * @return char* the calculated mime-type of the file in question
 */
String getMimeType(const String& fileName){
  // TODO: turn this into a mapped array at some point
  if (fileName.endsWith(".html")) {
    return "text/html";
  } else if (fileName.endsWith(".css")) {
    return "text/css";
  } else if (fileName.endsWith(".js")) {
    return "text/javascript";
  } else if (fileName.endsWith(".svg")) {
    return "image/svg+xml";
  }
  return "text/plain";
}
