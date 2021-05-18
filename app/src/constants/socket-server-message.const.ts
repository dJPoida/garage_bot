/**
 * These are the socket message types that the server (device) can send to the client (website)
 *
 * Make sure these match the values defined on the device firmware in `_config.h`
 */
export const SOCKET_SERVER_MESSAGE = {
  // A Pong sent back from the device to prove that it is still connected
  PONG: 1,

  // The status of the door has changed
  STATUS_CHANGE: 2,

  // The device config has changed
  CONFIG_CHANGE: 3,

  // The device is rebooting
  REBOOTING: 4,
} as const;
export type SOCKET_SERVER_MESSAGE = typeof SOCKET_SERVER_MESSAGE;
export type A_SOCKET_SERVER_MESSAGE = SOCKET_SERVER_MESSAGE[keyof SOCKET_SERVER_MESSAGE];
