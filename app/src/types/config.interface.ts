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

export const mapPayloadToConfig = (payload: Record<string, unknown>) => {
  return {
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
};
