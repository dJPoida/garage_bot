import React, { createContext } from "react";
import { A_DOOR_STATE, DOOR_STATE } from "../constants/door-state.const";
import { SOCKET_CLIENT_EVENT } from "../constants/socket-client-event.const";
import { A_SOCKET_CLIENT_STATE } from "../constants/socket-client-state.const";
import { IConfig } from "../types/config.interface";
import {
  A_SOCKET_SERVER_MESSAGE,
  SOCKET_SERVER_MESSAGE,
} from "../constants/socket-server-message.const";

import { socketClient } from "../helpers/socket-client.helper";

type DeviceProviderProps = {};
type DeviceProviderState = {
  socketClientState: A_SOCKET_CLIENT_STATE;
  error: null | Error;
  doorState: A_DOOR_STATE;
  config: IConfig;
};

type DeviceContextProps = Pick<
  DeviceProviderState,
  "socketClientState" | "error" | "doorState" | "config"
> & {};

export const DeviceContext = createContext<DeviceContextProps>(null as any);

export const DeviceContextConsumer = DeviceContext.Consumer;

/**
 * a uniform way of delivering props to a component that is dependent on the socketClient
 */
export class DeviceProvider extends React.Component<
  DeviceProviderProps,
  DeviceProviderState
> {
  constructor(props: DeviceProviderProps) {
    super(props);
    this.state = {
      socketClientState: socketClient.state,
      doorState: DOOR_STATE.UNKNOWN,
      error: socketClient.error,
      config: {
        mdnsName: null,
        networkDeviceName: null,
        wifiSSID: null,
        ipAddress: null,
        mqttBrokerAddress: null,
        mqttBrokerPort: null,
        mqttDeviceId: null,
        mqttUserName: null,
        mqttTopic: null,
        mqttStateTopic: null,
      },
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
    socketClient
      .on(SOCKET_CLIENT_EVENT.STATE_CHANGE, this.handleSocketClientStateChange)
      .on(SOCKET_CLIENT_EVENT.MESSAGE, this.handleSocketServerMessage);
  };

  /**
   * Tear down the event listeners
   */
  _unbindEvents = () => {
    socketClient
      .off(SOCKET_CLIENT_EVENT.STATE_CHANGE, this.handleSocketClientStateChange)
      .off(SOCKET_CLIENT_EVENT.MESSAGE, this.handleSocketServerMessage);
  };

  /**
   * Fired when the socket client state changes
   */
  handleSocketClientStateChange = (newState: A_SOCKET_CLIENT_STATE) => {
    this.setState({
      socketClientState: newState,
    });
  };

  /**
   * Fired when the socket client receives a message
   */
  handleSocketServerMessage = (
    message: A_SOCKET_SERVER_MESSAGE,
    payload: Record<string, unknown>
  ) => {
    switch (message) {
      case SOCKET_SERVER_MESSAGE.STATUS_CHANGE:
        this.setState({
          doorState: payload["door_state"] as A_DOOR_STATE,
        });
        return;
      case SOCKET_SERVER_MESSAGE.CONFIG_CHANGE:
        const newConfig: IConfig = {
          mdnsName: payload["mdns_name"] as string,
          networkDeviceName: payload["network_device_name"] as string,
          wifiSSID: payload["wifi_ssid"] as string,
          ipAddress: payload["ip_address"] as string,
          mqttBrokerAddress: payload["mqtt_broker_address"] as string,
          mqttBrokerPort: payload["mqtt_broker_port"] as number,
          mqttDeviceId: payload["mqtt_device_id"] as string,
          mqttUserName: payload["mqtt_username"] as string,
          mqttTopic: payload["mqtt_topic"] as string,
          mqttStateTopic: payload["mqtt_state_topic"] as string,
        };
        this.setState({
          config: newConfig,
        });
        return;
      default:
        console.error("Unhandled Server Message: ", message);
    }
  };

  /**
   * Fired when the device state changes
   */
  handleDeviceStateChange = (newState: { doorState: A_DOOR_STATE }) => {
    this.setState({ ...newState });
  };

  /**
   * Render
   */
  render() {
    const { children } = this.props;
    const { socketClientState, error, doorState, config } = this.state;
    return (
      <DeviceContext.Provider
        value={{
          socketClientState,
          error,
          doorState,
          config,
        }}
      >
        {children}
      </DeviceContext.Provider>
    );
  }
}
