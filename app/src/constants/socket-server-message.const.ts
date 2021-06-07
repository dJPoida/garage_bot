/**
 * These are the socket message types that the server (device) can send to the client (website)
 *
 * Make sure these match the values defined on the device firmware in `_config.h`
 */
export const SOCKET_SERVER_MESSAGE = {
  // The status of the door has changed
  STATUS_CHANGE: "SC",

  // The device config has changed
  CONFIG_CHANGE: "CC",

  // Sensor Data
  SENSOR_DATA: "SD",

  // The device is rebooting
  REBOOTING: "RB",
} as const;
export type SOCKET_SERVER_MESSAGE = typeof SOCKET_SERVER_MESSAGE;
export type A_SOCKET_SERVER_MESSAGE = SOCKET_SERVER_MESSAGE[keyof SOCKET_SERVER_MESSAGE];
