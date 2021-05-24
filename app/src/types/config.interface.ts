export interface IConfig extends Record<string, null | string | number> {
  mdnsName: null | string;
  networkDeviceName: null | string;
  wifiSSID: null | string;
  ipAddress: null | string;
  mqttBrokerAddress: null | string;
  mqttBrokerPort: null | number;
  mqttDeviceId: null | string;
  mqttUserName: null | string;
  mqttTopic: null | string;
  mqttStateTopic: null | string;
}
