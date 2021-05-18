import events from "events";
import { A_SOCKET_CLIENT_MESSAGE } from "../constants/socket-client-message.const";
import { SOCKET_CLIENT_EVENT } from "../constants/socket-client-event.const";
import { A_SOCKET_SERVER_MESSAGE } from "../constants/socket-server-message.const";
import { globals } from "./globals.helper";

const RECONNECTION_ATTEMPTS = 20;

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
  private _connected: boolean = false;
  private _connectionFailed: boolean = false;

  private _retryInfinitely: boolean = false;

  /**
   * @Constructor
   */
  constructor() {
    super();

    // Connect the socket
    setTimeout(this.connect, 0);
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
   * Is the socket currently connected
   */
  get connected() {
    return this._connected;
  }

  /**
   * True if the socket handler given up attempting to connect to the server
   */
  get connectionFailed() {
    return this._connectionFailed;
  }

  /**
   * Returns true if the socket handler is configured to never give up connecting
   * to the server.
   */
  get retryInfinitely() {
    return this._retryInfinitely;
  }

  set retryInfinitely(value) {
    this._retryInfinitely = value;
  }

  /**
   * Fired when the socket connects to the device
   */
  handleSocketOpen() {
    this._connected = true;
    this._connectionFailed = false;
    this.emit(SOCKET_CLIENT_EVENT.CONNECTED);
  }

  /**
   * Fired when the socket is disconnected from the device
   */
  handleSocketClose(e: CloseEvent) {
    this._connected = false;
    this.emit(SOCKET_CLIENT_EVENT.DISCONNECTED);
  }

  /**
   * Fired when the socket encounters an error
   */
  handleSocketError(e: Event) {
    console.log(e);
    // TODO: detect whether the error was a connection failure
  }

  /**
   * Fired when the client receives a message from the server
   */
  handleSocketMessage(e: MessageEvent) {
    const data = JSON.parse(e.data);

    const message: A_SOCKET_SERVER_MESSAGE = data.m;
    const payload: Record<string, unknown> = data.p ?? {};

    this.emit(SOCKET_CLIENT_EVENT.MESSAGE, message, payload);
  }

  /**
   * Call this method to establish the initial connection
   */
  connect = (): Promise<void> => {
    return new Promise(async () => {
      // Disconnect the existing socket
      if (this.connected && this._socket) {
        await this.disconnect();
      }

      // Notify Listeners that we're connecting to the client
      this.emit(SOCKET_CLIENT_EVENT.CONNECTING);

      this._connectionFailed = false;
      this._connected = false;
      this._socket = new WebSocket(this.host);

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
      // TODO: use a special close code?
      this._socket?.close();
    });
  };

  /**
   * Call this method when the connection has failed or is dropped
   */
  reconnect() {
    // TODO: reconnect
  }

  /**
   * Send a message back to the server (device)
   */
  sendMessage(
    message: A_SOCKET_CLIENT_MESSAGE,
    payload: Record<string, unknown>
  ) {
    const data = {
      message,
      payload: payload ?? {},
    };

    this._socket?.send(JSON.stringify(data));
  }
}

export const socketClient: SocketClient = SocketClient.getInstance();
