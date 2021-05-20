import React, { createContext } from "react";
import { SOCKET_CLIENT_EVENT } from "../constants/socket-client-event.const";
import { A_SOCKET_CLIENT_STATE } from "../constants/socket-client-state.const";

import { socketClient } from "../helpers/socket-client.helper";

type SocketClientProviderProps = {};
type SocketClientProviderState = {
  state: A_SOCKET_CLIENT_STATE;
  error: null | Error;
};

type SocketClientContextProps = Pick<
  SocketClientProviderState,
  "state" | "error"
> & {};

export const SocketClientContext = createContext<SocketClientContextProps>(
  null as any
);

export const SocketClientContextConsumer = SocketClientContext.Consumer;

/**
 * a uniform way of delivering props to a component that is dependent on the socketClient
 */
export class SocketClientProvider extends React.Component<
  SocketClientProviderProps,
  SocketClientProviderState
> {
  constructor(props: SocketClientProviderProps) {
    super(props);
    this.state = {
      state: socketClient.state,
      error: socketClient.error,
    };
    this._bindEvents();
  }

  /**
   * @inheritdoc
   */
  componentWillUnmount = () => {
    this._unbindEvents();
  };

  /**
   * Setup the event listeners
   */
  _bindEvents = () => {
    socketClient.on(
      SOCKET_CLIENT_EVENT.STATE_CHANGE,
      this.handleSocketClientStateChange
    );
  };

  /**
   * Tear down the event listeners
   */
  _unbindEvents = () => {
    socketClient.off(
      SOCKET_CLIENT_EVENT.STATE_CHANGE,
      this.handleSocketClientStateChange
    );
  };

  /**
   * Fired when the socket client state changes
   */
  handleSocketClientStateChange = (newState: A_SOCKET_CLIENT_STATE) => {
    console.log("ffs", newState);
    this.setState({
      state: newState,
    });
  };

  /**
   * Render
   */
  render() {
    const { children } = this.props;
    const { state, error } = this.state;
    return (
      <SocketClientContext.Provider
        value={{
          state,
          error,
        }}
      >
        {children}
      </SocketClientContext.Provider>
    );
  }
}
