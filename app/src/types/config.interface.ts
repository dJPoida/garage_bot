export interface IConfig {
  mdns_name: string,
  network_device_name: string,
  wifi_ssid: string,
  wifi_password: string,
  ip_address: string,
  mqtt_broker_address: string,
  mqtt_broker_port: number,
  mqtt_device_id: string,
  mqtt_username: string,
  mqtt_password: string,
  mqtt_topic: string,
  mqtt_state_topic: string,
}