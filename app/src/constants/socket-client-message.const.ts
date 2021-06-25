/**
 * These are the socket message types that the client (website) can send to the server (device)
 *
 * Make sure these match the values defined on the device firmware in `_config.h`
 */
export const SOCKET_CLIENT_MESSAGE = {
  // Simulate the pressing of a button
  BUTTON_PRESS: 'BP',

  // Set the threshold of a sensor
  SOCKET_CLIENT_MESSAGE_SET_SENSOR_THRESHOLD: 'ST',

  // Reboot the device
  REBOOT: 'RB',

  // Forget the WiFi credentials
  FORGET_WIFI: 'FW',

  // Reset the device to factory defaults
  RESET_TO_FACTORY_DEFAULTS: 'RF',
} as const;
export type SOCKET_CLIENT_MESSAGE = typeof SOCKET_CLIENT_MESSAGE;
export type A_SOCKET_CLIENT_MESSAGE =
  SOCKET_CLIENT_MESSAGE[keyof SOCKET_CLIENT_MESSAGE];
