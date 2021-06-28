/**
 * These are the states that the mqtt client can be in
 */
export const MQTT_CLIENT_STATE = {
  // Make sure these match the values defined on the device firmware in `helpers.h`
  DISABLED: 'DISABLED',
  ERROR: 'ERROR',
  CONNECTING: 'CONNECTING',
  CONNECTED: 'CONNECTED',
  DISCONNECTED: 'DISCONNECTED',
} as const;
export type MQTT_CLIENT_STATE = typeof MQTT_CLIENT_STATE;
export type AN_MQTT_CLIENT_STATE = MQTT_CLIENT_STATE[keyof MQTT_CLIENT_STATE];

export const MQTTClientStateDescriptionMap: { [K in AN_MQTT_CLIENT_STATE]: string } = {
  [MQTT_CLIENT_STATE.DISABLED]: 'Disabled',
  [MQTT_CLIENT_STATE.ERROR]: 'Error',
  [MQTT_CLIENT_STATE.CONNECTING]: 'Connecting',
  [MQTT_CLIENT_STATE.CONNECTED]: 'Connected',
  [MQTT_CLIENT_STATE.DISCONNECTED]: 'Disconnected',
};
