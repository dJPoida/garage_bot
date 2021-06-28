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
import { globals } from '../singletons/globals.singleton';
import { AN_MQTT_CLIENT_STATE, MQTT_CLIENT_STATE } from '../constants/mqtt-client-state.const';

type DeviceProviderProps = Record<string, unknown>;
type DeviceProviderState = {
  socketClientState: A_SOCKET_CLIENT_STATE;
  error: null | Error;
  doorState: A_DOOR_STATE;
  mqttClientState: AN_MQTT_CLIENT_STATE;
  mqttClientError: string,
  rebooting: boolean,
  config: IConfig;
  configChecksum: number;
  sensorData: ISensorData;
};

type DeviceContextProps = Pick<
  DeviceProviderState,
  | 'socketClientState'
  | 'error'
  | 'doorState'
  | 'mqttClientState'
  | 'mqttClientError'
  | 'rebooting'
  | 'config'
  | 'configChecksum'
  | 'sensorData'
> & {
  pressButton: (button: A_VIRTUAL_BUTTON) => void;
  reboot: () => void;
  forgetWiFi: () => void;
  resetToFactoryDefaults: () => void;
  setSensorThreshold: (sensorType: 'TOP' | 'BOTTOM', threshold: number) => void;
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
      mqttClientState: MQTT_CLIENT_STATE.DISABLED,
      mqttClientError: '',
      error: socketClient.error,
      configChecksum: 0,
      rebooting: false,
      config: {
        firmware_version: globals.firmwareVersion,
        mdns_name: null,
        device_name: null,
        wifi_ssid: null,
        ip_address: null,
        mac_address: null,
        mqtt_enabled: false,
        mqtt_broker_address: null,
        mqtt_broker_port: null,
        mqtt_device_id: null,
        mqtt_username: null,
        mqtt_password: null,
        mqtt_topic: null,
        mqtt_state_topic: null,
        top_ir_sensor_threshold: 0,
        bottom_ir_sensor_threshold: 0,
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
      rebooting: false,
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
          mqttClientState: payload.mqtt_client_state as AN_MQTT_CLIENT_STATE,
          mqttClientError: payload.mqtt_client_error as string,
        });
        return;

      // Config has changed
      case SOCKET_SERVER_MESSAGE.CONFIG_CHANGE: {
        const { configChecksum } = this.state;
        this.setState({
          config: mapPayloadToConfig(payload),
          configChecksum: configChecksum + 1,
        }, () => {
          const {
            config: {
              device_name,
              firmware_version,
            },
          } = this.state;
          globals.appTitle = device_name ?? globals.appTitle;
          globals.firmwareVersion = firmware_version ?? globals.firmwareVersion;
        });
        return;
      }

      // Device is rebooting
      case SOCKET_SERVER_MESSAGE.REBOOTING:
        this.setState({
          rebooting: true,
        });
        return;

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
   * Fired when the user wants to send a reboot command
   */
  handleReboot = (): void => {
    this.setState({ rebooting: true }, () => {
      socketClient.sendMessage(SOCKET_CLIENT_MESSAGE.REBOOT, {});
    });
  };


  /**
   * Fired when the user wants to reset the device to factory defaults
   */
  handleForgetWifi = (): void => {
    this.setState({ rebooting: true }, () => {
      socketClient.sendMessage(SOCKET_CLIENT_MESSAGE.FORGET_WIFI, {});
    });
  };


  /**
   * Fired when the user wants to reset the device to factory defaults
   */
  handleResetToFactoryDefaults = (): void => {
    this.setState({ rebooting: true }, () => {
      socketClient.sendMessage(SOCKET_CLIENT_MESSAGE.RESET_TO_FACTORY_DEFAULTS, {});
    });
  };


  /**
   * Fired when the user wants to set the threshold of one of the sensors
   * @param sensorType 'TOP' or 'BOTTOM'
   * @param threshold a number between 0 and 4095
   */
  handleSetSensorThreshold = (sensorType: 'TOP' | 'BOTTOM', threshold: number): void => {
    socketClient.sendMessage(SOCKET_CLIENT_MESSAGE.SOCKET_CLIENT_MESSAGE_SET_SENSOR_THRESHOLD, {
      s: sensorType,
      t: threshold,
    });
  }


  /**
   * Render
   */
  render(): JSX.Element {
    const { children } = this.props;
    const {
      socketClientState,
      error,
      doorState,
      mqttClientState,
      mqttClientError,
      rebooting,
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
          mqttClientState,
          mqttClientError,
          rebooting,
          config,
          configChecksum,
          sensorData,
          pressButton: this.handleButtonPress,
          reboot: this.handleReboot,
          forgetWiFi: this.handleForgetWifi,
          resetToFactoryDefaults: this.handleResetToFactoryDefaults,
          setSensorThreshold: this.handleSetSensorThreshold,
        }}
      >
        {children}
      </DeviceContext.Provider>
    );
  }
}
