import React, { createContext } from "react";
import { A_DOOR_STATE, DOOR_STATE } from "../constants/door-state.const";
import { SOCKET_CLIENT_EVENT } from "../constants/socket-client-event.const";
import { A_SOCKET_CLIENT_STATE } from "../constants/socket-client-state.const";
import { IConfig, mapPayloadToConfig } from "../types/config.interface";
import {
  A_SOCKET_SERVER_MESSAGE,
  SOCKET_SERVER_MESSAGE,
} from "../constants/socket-server-message.const";

import { socketClient } from "../helpers/socket-client.helper";
import {
  ISensorData,
  mapPayloadToSensorData,
} from "../types/sensor-data.interface";

type DeviceProviderProps = {};
type DeviceProviderState = {
  socketClientState: A_SOCKET_CLIENT_STATE;
  error: null | Error;
  doorState: A_DOOR_STATE;
  config: IConfig;
  sensorData: ISensorData;
};

type DeviceContextProps = Pick<
  DeviceProviderState,
  "socketClientState" | "error" | "doorState" | "config" | "sensorData"
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
      sensorData: {
        topIRSensorDetected: false,
        topIRSensorAverageAmbientReading: 0,
        topIRSensorAverageActiveReading: 0,
        bottomIRSensorDetected: false,
        bottomIRSensorAverageAmbientReading: 0,
        bottomIRSensorAverageActiveReading: 0,
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
      // Device Status has changed
      case SOCKET_SERVER_MESSAGE.STATUS_CHANGE:
        this.setState({
          doorState: payload["door_state"] as A_DOOR_STATE,
        });
        return;

      // Config has changed
      case SOCKET_SERVER_MESSAGE.CONFIG_CHANGE:
        this.setState({
          config: mapPayloadToConfig(payload),
        });
        return;

      // Device Status has changed
      case SOCKET_SERVER_MESSAGE.SENSOR_DATA:
        console.log(mapPayloadToSensorData(payload));
        this.setState({
          sensorData: mapPayloadToSensorData(payload),
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
    const {
      socketClientState,
      error,
      doorState,
      config,
      sensorData,
    } = this.state;
    return (
      <DeviceContext.Provider
        value={{
          socketClientState,
          error,
          doorState,
          config,
          sensorData,
        }}
      >
        {children}
      </DeviceContext.Provider>
    );
  }
}
