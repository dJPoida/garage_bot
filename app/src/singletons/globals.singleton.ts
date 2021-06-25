declare const __ENVIRONMENT__: string;
declare const __VERSION__: string;
declare const __TITLE__: string;
declare const __HOMEPAGE__: string;
declare const __DEVICE_ADDRESS__: string;

declare const firmwareVersion: string;
declare const deviceAddress: string;
declare const deviceName: string;

export const globals = {
  environment: __ENVIRONMENT__,
  development: __ENVIRONMENT__ === 'development',
  production: __ENVIRONMENT__ !== 'development',
  appTitle: deviceName === '%DEVICE_NAME%' ? __TITLE__ : deviceName,
  version: __VERSION__,
  homepage: __HOMEPAGE__,
  firmwareVersion: firmwareVersion === '%FIRMWARE_VERSION%' ? '0.0.0' : firmwareVersion,
  deviceAddress:
    __ENVIRONMENT__ === 'development' ? __DEVICE_ADDRESS__ : deviceAddress,
};
