/**
 * These close codes are used to establish reasons for the websocket closing
 * @see https://developer.mozilla.org/en-US/docs/Web/API/CloseEvent
 */
export const SOCKET_CLIENT_CLOSE_CODE = {
  // Normal closure; the connection successfully completed whatever purpose for which it was created.
  NORMAL_CLOSURE: 1000,

  // The endpoint is going away, either because of a server failure or because the browser is navigating away from the page that opened the connection.
  GOING_AWAY: 1001,

  // The endpoint is terminating the connection due to a protocol error.
  PROTOCOL_ERROR: 1002,

  // The connection is being terminated because the endpoint received data of a type it cannot accept (for example, a text-only endpoint received binary data).
  UNSUPPORTED_DATA: 1003,

  // Indicates that no status code was provided even though one was expected.
  NO_STATUS_RECEIVED: 1005,

  // Used to indicate that a connection was closed abnormally (that is, with no close frame being sent) when a status code is expected.
  ABNORMAL_CLOSURE: 1006,

  // The endpoint is terminating the connection because a message was received that contained inconsistent data (e.g., non-UTF-8 data within a text message).
  INVALID_FRAME_PAYLOAD_DATA: 1007,

  // The endpoint is terminating the connection because it received a message that violates its policy. This is a generic status code, used when codes 1003 and 1009 are not suitable.
  POLICY_VIOLATION: 1008,

  // The endpoint is terminating the connection because a data frame was received that is too large.
  MESSAGE_TOO_BIG: 1009,

  // The client is terminating the connection because it expected the server to negotiate one or more extension, but the server didn't.
  MISSING_EXTENSION: 1010,

  // The server is terminating the connection because it encountered an unexpected condition that prevented it from fulfilling the request.
  INTERNAL_ERROR: 1011,

  // The server is terminating the connection because it is restarting.
  SERVICE_RESTART: 1012,

  // The server is terminating the connection due to a temporary condition, e.g. it is overloaded and is casting off some of its clients.
  TRY_AGAIN_LATER: 1013,

  // The server was acting as a gateway or proxy and received an invalid response from the upstream server. This is similar to 502 HTTP Status Code.
  BAD_GATEWAY: 1014,

  // Indicates that the connection was closed due to a failure to perform a TLS handshake (e.g., the server certificate can't be verified).
  TLS_HANDSHAKE: 1015,

  // Pre-existing connection closed in anticipation of new connection
  CLOSING_FOR_RECONNECT: 4000,
} as const;
export type SOCKET_CLIENT_CLOSE_CODE = typeof SOCKET_CLIENT_CLOSE_CODE;
export type A_SOCKET_CLIENT_CLOSE_CODE =
  SOCKET_CLIENT_CLOSE_CODE[keyof SOCKET_CLIENT_CLOSE_CODE];

export const SocketClientCloseCodeDescriptionMap: {
  [K in A_SOCKET_CLIENT_CLOSE_CODE]: string;
} = {
  [SOCKET_CLIENT_CLOSE_CODE.NORMAL_CLOSURE]: 'Closed Normally',
  [SOCKET_CLIENT_CLOSE_CODE.GOING_AWAY]:
    'The endpoint is going away, either because of a server failure or because the browser is navigating away from the page that opened the connection.',
  [SOCKET_CLIENT_CLOSE_CODE.PROTOCOL_ERROR]:
    'The endpoint is terminating the connection due to a protocol error.',
  [SOCKET_CLIENT_CLOSE_CODE.UNSUPPORTED_DATA]:
    'The connection is being terminated because the endpoint received data of a type it cannot accept.',
  [SOCKET_CLIENT_CLOSE_CODE.NO_STATUS_RECEIVED]:
    'No status code was provided even though one was expected.',
  [SOCKET_CLIENT_CLOSE_CODE.ABNORMAL_CLOSURE]:
    'A connection was closed abnormally (that is, with no close frame being sent) when a status code is expected.',
  [SOCKET_CLIENT_CLOSE_CODE.INVALID_FRAME_PAYLOAD_DATA]:
    'The endpoint is terminating the connection because a message was received that contained inconsistent data (e.g., non-UTF-8 data within a text message).',
  [SOCKET_CLIENT_CLOSE_CODE.POLICY_VIOLATION]:
    'The endpoint is terminating the connection because it received a message that violates its policy.',
  [SOCKET_CLIENT_CLOSE_CODE.MESSAGE_TOO_BIG]:
    'The endpoint is terminating the connection because a data frame was received that is too large.',
  [SOCKET_CLIENT_CLOSE_CODE.MISSING_EXTENSION]:
    "The client is terminating the connection because it expected the server to negotiate one or more extension, but the server didn't.",
  [SOCKET_CLIENT_CLOSE_CODE.INTERNAL_ERROR]:
    'The server is terminating the connection because it encountered an unexpected condition that prevented it from fulfilling the request.',
  [SOCKET_CLIENT_CLOSE_CODE.SERVICE_RESTART]:
    'The server is terminating the connection because it is restarting.',
  [SOCKET_CLIENT_CLOSE_CODE.TRY_AGAIN_LATER]:
    'The server is terminating the connection due to a temporary condition, e.g. it is overloaded and is casting off some of its clients.',
  [SOCKET_CLIENT_CLOSE_CODE.BAD_GATEWAY]:
    'The server was acting as a gateway or proxy and received an invalid response from the upstream server.',
  [SOCKET_CLIENT_CLOSE_CODE.TLS_HANDSHAKE]:
    'Indicates that the connection was closed due to a failure to perform a TLS handshake.',
  [SOCKET_CLIENT_CLOSE_CODE.CLOSING_FOR_RECONNECT]:
    'Pre-existing connection closed in anticipation of new connection',
};
