/**
 * These are the states that the socket client be in
 */
export const SOCKET_CLIENT_STATE = {
  DISCONNECTED: 'DISCONNECTED',
  CONNECTING: 'CONNECTING',
  CONNECTED: 'CONNECTED',
  ERROR: 'ERROR',
} as const;
export type SOCKET_CLIENT_STATE = typeof SOCKET_CLIENT_STATE;
export type A_SOCKET_CLIENT_STATE =
  SOCKET_CLIENT_STATE[keyof SOCKET_CLIENT_STATE];
