import React, { createContext } from 'react';

import { IConfig, mapPayloadToConfig } from '../types/config.interface';
import { ISensorData, mapPayloadToSensorData } from '../types/sensor-data.interface';

import { socketClient } from '../singletons/socket-client.singleton';

import { A_DOOR_STATE, DOOR_STATE } from '../constants/door-state.const';
import { A_SOCKET_CLIENT_STATE } from '../constants/socket-client-state.const';
import { A_SOCKET_SERVER_MESSAGE, SOCKET_SERVER_MESSAGE } from '../constants/socket-server-message.const';
import { A_VIRTUAL_BUTTON } from '../constants/device-button.const';
import { SOCKET_CLIENT_EVENT } from '../constants/socket-client-event.const';
import { SOCKET_CLIENT_MESSAGE } from '../constants/socket-client-message.const';

type DeviceProviderProps = Record<string, unknown>;
type DeviceProviderState = {
  socketClientState: A_SOCKET_CLIENT_STATE;
  error: null | Error;
  doorState: A_DOOR_STATE;
  config: IConfig;
  configChecksum: number;
  sensorData: ISensorData;
};

type DeviceContextProps = Pick<
  DeviceProviderState,
  | 'socketClientState'
  | 'error'
  | 'doorState'
  | 'config'
  | 'configChecksum'
  | 'sensorData'
> & {
  pressButton: (button: A_VIRTUAL_BUTTON) => void;
};

// eslint-disable-next-line @typescript-eslint/no-explicit-any
export const DeviceContext = createContext<DeviceContextProps>(null as any);

export const DeviceContextConsumer = DeviceContext.Consumer;

/**
 * a uniform way of delivering props to a component that is dependent on the socketClient
 */
export class DeviceProvider extends React.Component<DeviceProviderProps, DeviceProviderState> {
  constructor(props: DeviceProviderProps) {
    super(props);
    this.state = {
      socketClientState: socketClient.state,
      doorState: DOOR_STATE.UNKNOWN,
      error: socketClient.error,
      configChecksum: 0,
      config: {
        mdns_name: null,
        network_device_name: null,
        wifi_ssid: null,
        ip_address: null,
        mqtt_enabled: false,
        mqtt_broker_address: null,
        mqtt_broker_port: null,
        mqtt_device_id: null,
        mqtt_username: null,
        mqtt_password: null,
        mqtt_topic: null,
        mqtt_state_topic: null,
      },
      sensorData: {
        topIRSensorDetected: false,
        topIRSensorAverageAmbientReading: 0,
        topIRSensorAverageActiveReading: 0,
        bottomIRSensorDetected: false,
        bottomIRSensorAverageAmbientReading: 0,
        bottomIRSensorAverageActiveReading: 0,
        available_memory: 0,
      },
    };
    this._bindEvents();
  }

  /**
   * @inheritdoc
   */
  componentWillUnmount = (): void => {
    this._unbindEvents();
  };

  /**
   * Setup the event listeners
   */
  _bindEvents = (): void => {
    socketClient
      .on(SOCKET_CLIENT_EVENT.STATE_CHANGE, this.handleSocketClientStateChange)
      .on(SOCKET_CLIENT_EVENT.MESSAGE, this.handleSocketServerMessage);
  };

  /**
   * Tear down the event listeners
   */
  _unbindEvents = (): void => {
    socketClient
      .off(SOCKET_CLIENT_EVENT.STATE_CHANGE, this.handleSocketClientStateChange)
      .off(SOCKET_CLIENT_EVENT.MESSAGE, this.handleSocketServerMessage);
  };

  /**
   * Fired when the socket client state changes
   */
  handleSocketClientStateChange = (newState: A_SOCKET_CLIENT_STATE): void => {
    this.setState({
      socketClientState: newState,
    });
  };

  /**
   * Fired when the socket client receives a message
   */
  handleSocketServerMessage = (
    message: A_SOCKET_SERVER_MESSAGE,
    payload: Record<string, unknown>,
  ): void => {
    switch (message) {
      // Device Status has changed
      case SOCKET_SERVER_MESSAGE.STATUS_CHANGE:
        this.setState({
          doorState: payload.door_state as A_DOOR_STATE,
        });
        return;

      // Config has changed
      case SOCKET_SERVER_MESSAGE.CONFIG_CHANGE: {
        const { configChecksum } = this.state;
        this.setState({
          config: mapPayloadToConfig(payload),
          configChecksum: configChecksum + 1,
        });
        return;
      }

      // Device Status has changed
      case SOCKET_SERVER_MESSAGE.SENSOR_DATA:
        this.setState({
          sensorData: mapPayloadToSensorData(payload),
        });
        return;

      default:
        console.error('Unhandled Server Message: ', message);
    }
  };

  /**
   * Fired when the device state changes
   */
  handleDeviceStateChange = (newState: { doorState: A_DOOR_STATE }): void => {
    this.setState({ ...newState });
  };

  /**
   * Fired when the user presses the "activate door" button
   */
  handleButtonPress = (button: A_VIRTUAL_BUTTON): void => {
    socketClient.sendMessage(SOCKET_CLIENT_MESSAGE.BUTTON_PRESS, {
      b: button,
    });
  };

  /**
   * Render
   */
  render(): JSX.Element {
    const { children } = this.props;
    const {
      socketClientState,
      error,
      doorState,
      config,
      configChecksum,
      sensorData,
    } = this.state;
    return (
      <DeviceContext.Provider
        value={{
          socketClientState,
          error,
          doorState,
          config,
          configChecksum,
          sensorData,
          pressButton: this.handleButtonPress,
        }}
      >
        {children}
      </DeviceContext.Provider>
    );
  }
}
