export interface IConfig
  extends Record<string, null | string | number | boolean> {
  mdns_name: null | string;
  network_device_name: null | string;
  wifi_ssid: null | string;
  ip_address: null | string;
  mqtt_enabled: boolean;
  mqtt_broker_address: null | string;
  mqtt_broker_port: null | number;
  mqtt_device_id: null | string;
  mqtt_username: null | string;
  mqtt_password: null | string;
  mqtt_topic: null | string;
  mqtt_state_topic: null | string;
}

export const mapPayloadToConfig = (payload: Record<string, unknown>) => {
  return {
    mdns_name: payload["mdns_name"] as string,
    network_device_name: payload["network_device_name"] as string,
    wifi_ssid: payload["wifi_ssid"] as string,
    ip_address: payload["ip_address"] as string,
    mqtt_enabled: (payload["mqtt_enabled"] ?? false) as boolean,
    mqtt_broker_address: payload["mqtt_broker_address"] as string,
    mqtt_broker_port: payload["mqtt_broker_port"] as number,
    mqtt_device_id: payload["mqtt_device_id"] as string,
    mqtt_username: payload["mqtt_username"] as string,
    mqtt_password: payload["mqtt_password"] as string,
    mqtt_topic: payload["mqtt_topic"] as string,
    mqtt_state_topic: payload["mqtt_state_topic"] as string,
  };
};
