/**
 * These are the socket message types that the client (website) can send to the server (device)
 *
 * Make sure these match the values defined on the device firmware in `_config.h`
 */
export const SOCKET_CLIENT_MESSAGE = {
  // Send a PING to the device to check if it's still there
  PING: "PI",

  // Simulate the pressing of a button
  PRESS_BUTTON: "BD",

  // Simulate the release of the button
  RELEASE_BUTTON: "BU",

  // Restart the device
  REBOOT: "RS",
} as const;
export type SOCKET_CLIENT_MESSAGE = typeof SOCKET_CLIENT_MESSAGE;
export type A_SOCKET_CLIENT_MESSAGE = SOCKET_CLIENT_MESSAGE[keyof SOCKET_CLIENT_MESSAGE];
