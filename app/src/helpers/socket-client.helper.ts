import events from "events";
import { A_SOCKET_CLIENT_MESSAGE } from "../constants/socket-client-message.const";
import { SOCKET_CLIENT_EVENT } from "../constants/socket-client-event.const";
import { A_SOCKET_SERVER_MESSAGE } from "../constants/socket-server-message.const";
import { globals } from "./globals.helper";
import {
  A_SOCKET_CLIENT_STATE,
  SOCKET_CLIENT_STATE,
} from "../constants/socket-client-state.const";
import {
  A_SOCKET_CLIENT_CLOSE_CODE,
  SocketClientCloseCodeDescriptionMap,
  SOCKET_CLIENT_CLOSE_CODE,
} from "../constants/socket-client-close-code.const";

const RECONNECTION_ATTEMPTS = 20;
const PING_PONG_TIMEOUT = 2000;
const MAX_MISSED_PINGS = 3;

let socketClientInstance: SocketClient;

/**
 * A singleton class for managing communication between the client (website) and the server (device)
 */
class SocketClient extends events.EventEmitter {
  // The host can be overridden for local device modes
  private readonly host: string = `ws://${
    globals.development ? globals.deviceAddress : window.location.host
  }/ws`;

  private _socket: null | WebSocket = null;
  private _state: A_SOCKET_CLIENT_STATE = SOCKET_CLIENT_STATE.DISCONNECTED;
  private _connectionFailed: boolean = false;
  private _error: null | Error = null;

  private _pongTimeout: undefined | ReturnType<typeof setTimeout>;
  private _missedPings: number = 0;

  /**
   * @var retryInfinitely whether the socket client should attempt to maintain the connection to the device at all costs
   */
  public retryInfinitely: boolean = true;

  /**
   * @Constructor
   */
  constructor() {
    super();

    // Connect the socket
    this.connect();
  }

  /**
   * Create a new instance of the socket handler if one does not already exist
   */
  static getInstance = (): SocketClient => {
    if (!socketClientInstance) {
      socketClientInstance = new SocketClient();
    }
    return socketClientInstance;
  };

  /**
   * The state of the socket client
   */
  get state(): A_SOCKET_CLIENT_STATE {
    return this._state;
  }

  /**
   * The most recent error that the socket client encountered
   */
  get error(): null | Error {
    return this._error;
  }

  /**
   * Change the state of the socket client
   */
  private setState = (newState: A_SOCKET_CLIENT_STATE) => {
    this._state = newState;
    this.emit(SOCKET_CLIENT_EVENT.STATE_CHANGE, newState);
  };

  /**
   * Fired when the socket connects to the device
   */
  handleSocketOpen = () => {
    this._connectionFailed = false;
    this._error = null;
    this.setState(SOCKET_CLIENT_STATE.CONNECTED);
    this.heartBeat();
    console.info("Socket Connected.");
  };

  /**
   * Fired when the socket is disconnected from the device
   */
  handleSocketClose = (e: CloseEvent) => {
    // If the closure was an error, log it otherwise reset the class
    if (e.code > 1001 && e.code < 4000) {
      console.error(
        new Error(
          SocketClientCloseCodeDescriptionMap[
            e.code as A_SOCKET_CLIENT_CLOSE_CODE
          ]
        )
      );
    } else {
      console.info("Socket Closed.");
    }

    this._error = null;
    this.setState(SOCKET_CLIENT_STATE.DISCONNECTED);

    // Clear the PingPong timeouts
    if (this._pongTimeout) {
      clearTimeout(this._pongTimeout);
      this._pongTimeout = undefined;
    }
  };

  /**
   * Fired when the socket encounters an error
   */
  handleSocketError = (e: Event) => {
    switch (this.state) {
      case SOCKET_CLIENT_STATE.CONNECTING:
        this._error = new Error("Failed to connect to the device!");
        break;
      case SOCKET_CLIENT_STATE.CONNECTED:
        this._error = new Error("Connection to the device was Lost!");
        break;
      default:
        this._error = new Error("An unexpected error has occurred!");
    }
    this.setState(SOCKET_CLIENT_STATE.ERROR);
    console.error(this.error);
  };

  /**
   * Fired when the client receives a message from the server
   */
  handleSocketMessage = (e: MessageEvent) => {
    const data = JSON.parse(e.data);

    const message: A_SOCKET_SERVER_MESSAGE = data.m;
    const payload: Record<string, unknown> = data.p ?? {};

    this.emit(SOCKET_CLIENT_EVENT.MESSAGE, message, payload);
  };

  /**
   * Call this method to establish the initial connection
   */
  connect = (): Promise<void> => {
    return new Promise(async () => {
      // Disconnect an  existing socket
      if (this.state === SOCKET_CLIENT_STATE.CONNECTED && this._socket) {
        this._socket?.close(-1);
      }

      // Notify Listeners that we're connecting to the client
      this.setState(SOCKET_CLIENT_STATE.CONNECTING);

      // Reset some of the state values
      this._connectionFailed = false;
      this._error = null;
      this._socket = new WebSocket(this.host);

      // Bind the websocket event listeners
      this._socket.onopen = this.handleSocketOpen;
      this._socket.onclose = this.handleSocketClose;
      this._socket.onmessage = this.handleSocketMessage;
      this._socket.onerror = this.handleSocketError;
    });
  };

  /**
   * Disconnect from the WebSocket
   */
  disconnect = (): Promise<void> => {
    return new Promise(() => {
      this._socket?.close(
        SOCKET_CLIENT_CLOSE_CODE.NORMAL_CLOSURE,
        "Closed by user"
      );
    });
  };

  /**
   * Call this method when the connection has failed or is dropped
   */
  reconnect = () => {
    // TODO: reconnect
  };

  /**
   * Send a message back to the server (device)
   */
  sendMessage = (
    message: A_SOCKET_CLIENT_MESSAGE,
    payload: Record<string, unknown>
  ) => {
    const data = {
      message,
      payload: payload ?? {},
    };

    this._socket?.send(JSON.stringify(data));
  };

  /**
   * Once connected, the heartbeat will periodically send a PING to the server
   * if the PONG is not received by the timeout
   */
  heartBeat = () => {
    this._missedPings = 0;
    this._pongTimeout = setTimeout(() => {
      this._missedPings += 1;
      if (this._missedPings >= MAX_MISSED_PINGS) {
        this.disconnect();
      } else {
        this.heartBeat();
      }
    }, PING_PONG_TIMEOUT);
  };
}

export const socketClient: SocketClient = SocketClient.getInstance();
