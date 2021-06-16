import events from 'events';
import { A_SOCKET_CLIENT_MESSAGE } from '../constants/socket-client-message.const';
import { SOCKET_CLIENT_EVENT } from '../constants/socket-client-event.const';
import { A_SOCKET_SERVER_MESSAGE, SOCKET_SERVER_MESSAGE } from '../constants/socket-server-message.const';
import { globals } from './globals.singleton';
import { A_SOCKET_CLIENT_STATE, SOCKET_CLIENT_STATE } from '../constants/socket-client-state.const';
import { A_SOCKET_CLIENT_CLOSE_CODE, SocketClientCloseCodeDescriptionMap, SOCKET_CLIENT_CLOSE_CODE } from '../constants/socket-client-close-code.const';
import { pageActivity, PAGE_ACTIVITY_EVENT } from './page-activity.singleton';

const PING_INTERVAL = 2000;
const PONG_TIMEOUT = 2000;
const MAX_MISSED_PINGS = 3;

// These need to match the values in the device `_config.h` file
const MAX_SOCKET_SERVER_MESSAGE_SIZE = 1024;
const MAX_SOCKET_CLIENT_MESSAGE_SIZE = 256;

let socketClientInstance: SocketClient;

/**
 * A singleton class for managing communication between the client (website) and the server (device)
 */
class SocketClient extends events.EventEmitter {
  // The host can be overridden for local device modes
  private readonly host: string = `ws://${globals.development ? globals.deviceAddress : window.location.host}/ws`;

  private _socket: null | WebSocket = null;
  private _state: A_SOCKET_CLIENT_STATE = SOCKET_CLIENT_STATE.DISCONNECTED;
  private _error: null | Error = null;

  private _pingTimeout: undefined | ReturnType<typeof setTimeout>;
  private _pongTimeout: undefined | ReturnType<typeof setTimeout>;
  private _missedPings = 0;

  /**
   * @var keepConnectionOpen whether the socket client should attempt to maintain the connection to the device at all costs
   */
  public keepConnectionOpen = true;

  /**
   * @Constructor
   */
  constructor() {
    super();

    // Bind events
    this.bindEvents();

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
   * Bind the event listeners that this class cares about
   */
  private bindEvents = () => {
    pageActivity.on(
      PAGE_ACTIVITY_EVENT.USER_ACTIVE_CHANGE,
      this.handleUserActiveChanged.bind(this),
    );
  };


  /**
   * Once connected, a PING is periodically sent to the server.
   * If the PONG is not received, the connection will be severed
   * and an attempt to reconnect will commence.
   */
  private sendPing = async () => {
    if (this._pingTimeout) {
      clearTimeout(this._pingTimeout);
    }
    if (this._pongTimeout) {
      clearTimeout(this._pongTimeout);
    }

    // Send the ping after the PING_INTERVAL
    this._pingTimeout = setTimeout(() => {
      this._pingTimeout = undefined;

      // Send the PING message
      if (this._socket?.readyState === 1) {
        this._socket.send('PING');
      }

      // Prepare a timeout
      this._pongTimeout = setTimeout(() => {
        this.handlePongReceived(false);
      }, PONG_TIMEOUT);
    }, PING_INTERVAL);
  };


  /**
   * Fired when the page becomes visible or hidden
   */
  private handleUserActiveChanged = (newUserActive: boolean) => {
    // Reconnect if the device is disconnected and the user returns to the page
    if (
      newUserActive
      && (
        [
          SOCKET_CLIENT_STATE.DISCONNECTED,
          SOCKET_CLIENT_STATE.ERROR,
        ] as A_SOCKET_CLIENT_STATE[]
      ).includes(this.state)
    ) {
      this.reconnect();
    }
  };


  /**
   * Fired when a PONG message is received in response to a PING message
   * Also fired when a PONG message is missed
   *
   * @param received whether the pong was received or missed
   */
  private handlePongReceived = async (received = true) => {
    if (this._pongTimeout) {
      clearTimeout(this._pongTimeout);
    }
    this._pongTimeout = undefined;

    // If the pong was received
    if (received) {
      this._missedPings = 0;

      // Send another ping
      this.sendPing();
    }

    // If the pong was missed
    else {
      this._missedPings += 1;

      if (this._missedPings >= MAX_MISSED_PINGS) {
        this.disconnect(
          SOCKET_CLIENT_CLOSE_CODE.CLOSING_FOR_RECONNECT,
          'Missed Heartbeat',
        );
      } else {
        // Send another ping
        this.sendPing();
      }
    }
  };


  /**
   * Fired when the socket connects to the device
   */
  private handleSocketOpen = () => {
    this._error = null;
    this.setState(SOCKET_CLIENT_STATE.CONNECTED);
    this.sendPing();
    console.info('Socket Connected.');
  };


  /**
   * Fired when the socket is disconnected from the device
   */
  private handleSocketClose = (e: CloseEvent) => {
    // Clear the PingPong timeouts
    if (this._pingTimeout) {
      clearTimeout(this._pingTimeout);
    }
    this._pingTimeout = undefined;
    if (this._pongTimeout) {
      clearTimeout(this._pongTimeout);
    }
    this._pongTimeout = undefined;

    // If the closure was an error, log it otherwise reset the class
    if (e.code > 1001 && e.code < 4000) {
      // Spit out a connection lost error
      console.error(
        new Error(
          SocketClientCloseCodeDescriptionMap[
            e.code as A_SOCKET_CLIENT_CLOSE_CODE
          ],
        ),
      );

      this._error = new Error('Connection Lost!');

      // Attempt to to reconnect
      this.reconnect();
    } else {
      // This is a graceful close. Just shut the connection down
      console.info('Socket Closed.');
      this._error = null;
      this.setState(SOCKET_CLIENT_STATE.DISCONNECTED);
    }
  };


  /**
   * Fired when the socket encounters an error
   */
  private handleSocketError = () => {
    switch (this.state) {
      case SOCKET_CLIENT_STATE.CONNECTING:
        this._error = new Error('Failed to connect to the device!');
        break;
      case SOCKET_CLIENT_STATE.CONNECTED:
        this._error = new Error('Connection to the device was Lost!');
        break;
      default:
        this._error = new Error('An unexpected error has occurred!');
    }
    this.setState(SOCKET_CLIENT_STATE.ERROR);
    console.error(this.error);
  };


  /**
   * Fired when the client receives a message from the server
   */
  private handleSocketMessage = (e: MessageEvent) => {
    // First, check to see if the message data is a "PONG" in response to our heartbeat ping
    if (e.data === 'PONG') {
      this.handlePongReceived();
    }

    // Otherwise attempt to parse the message data as JSON
    else {
      const data = JSON.parse(e.data);

      const message: A_SOCKET_SERVER_MESSAGE = data.m;
      const payload: Record<string, unknown> = data.p ?? {};

      // Keep the page alive if we receive a reboot message
      if (message === SOCKET_SERVER_MESSAGE.REBOOTING) {
        pageActivity.poke();
      }

      this.emit(SOCKET_CLIENT_EVENT.MESSAGE, message, payload);
    }
  };


  /**
   * Call this method to establish the initial connection
   */
  public connect = async () => {
    // Disconnect an  existing socket
    if (this.state === SOCKET_CLIENT_STATE.CONNECTED && this._socket) {
      this._socket?.close(SOCKET_CLIENT_CLOSE_CODE.CLOSING_FOR_RECONNECT);
    }

    // Notify Listeners that we're connecting to the client
    this.setState(SOCKET_CLIENT_STATE.CONNECTING);

    // Reset some of the state values
    this._error = null;
    this._socket = new WebSocket(this.host);

    // Bind the websocket event listeners
    this._socket.onopen = this.handleSocketOpen;
    this._socket.onclose = this.handleSocketClose;
    this._socket.onmessage = this.handleSocketMessage;
    this._socket.onerror = this.handleSocketError;
  };


  /**
   * Call this method when the connection has failed or is dropped
   */
  private reconnect = () => {
    // Only attempt to reconnect if the page is visible and we want to keep the connection open
    if (this.keepConnectionOpen && pageActivity.userActive) {
      console.info('Attempting to re-connect...');
      this.connect();
    } else {
      console.info('User is inactive. No point re-connecting.');
      this.setState(SOCKET_CLIENT_STATE.DISCONNECTED);
    }
  };


  /**
   * Disconnect from the WebSocket
   */
  public disconnect = async (
    code: A_SOCKET_CLIENT_CLOSE_CODE = SOCKET_CLIENT_CLOSE_CODE.NORMAL_CLOSURE,
    reason = 'Closed by user',
  ) => {
    this._socket?.close(code, reason);
  };


  /**
   * Send a message back to the server (device)
   */
  public sendMessage = (
    message: A_SOCKET_CLIENT_MESSAGE,
    payload: Record<string, unknown>,
  ) => {
    const data = {
      m: message,
      p: payload ?? {},
    };

    const jsonEncodedMessage = JSON.stringify(data);
    const messageLength = jsonEncodedMessage.length;
    if (messageLength > MAX_SOCKET_CLIENT_MESSAGE_SIZE) {
      throw new Error(
        `Failed to send message to device. MAX_SOCKET_CLIENT_MESSAGE_SIZE exceeded. Max = ${MAX_SOCKET_CLIENT_MESSAGE_SIZE}, Message = ${messageLength}.`,
      );
    }

    this._socket?.send(jsonEncodedMessage);
  };
}

export const socketClient: SocketClient = SocketClient.getInstance();
