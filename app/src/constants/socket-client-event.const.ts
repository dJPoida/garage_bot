/**
 * These are the events that the socket client can fire to any subscribed listeners
 */
export const SOCKET_CLIENT_EVENT = {
  MESSAGE: "MESSAGE",
  STATE_CHANGE: "STATE_CHANGE",
} as const;
export type SOCKET_CLIENT_EVENT = typeof SOCKET_CLIENT_EVENT;
export type A_SOCKET_CLIENT_EVENT =
  SOCKET_CLIENT_EVENT[keyof SOCKET_CLIENT_EVENT];
