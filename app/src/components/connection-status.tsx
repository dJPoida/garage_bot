import React, { useContext } from 'react';
import { SOCKET_CLIENT_STATE } from '../constants/socket-client-state.const';

import { DeviceContext } from '../providers/device.provider';
import { AppTitle } from './app-title';

export const ConnectionStatus: React.FC = () => {
  const { rebooting, socketClientState } = useContext(DeviceContext);

  // Device is rebooting
  if (rebooting) {
    return (
      <div className="connection-status please-wait">
        <AppTitle />
        <div className="status-description">
          <span className="spinner" />
          <span>Device is rebooting...</span>
        </div>
      </div>
    );
  }

  // Connecting to the device
  if (socketClientState === SOCKET_CLIENT_STATE.CONNECTING) {
    return (
      <div className="connection-status please-wait">
        <AppTitle />
        <div className="status-description">
          <span className="spinner" />
          <span>Connecting to device...</span>
        </div>
      </div>
    );
  }

  // Disconnected
  if (socketClientState === SOCKET_CLIENT_STATE.DISCONNECTED) {
    return (
      <div className="connection-status disconnected">
        <AppTitle />
        <div className="status-description">
          <span className="icon-alert" />
          <span>Disconnected!</span>
        </div>
        <br />
        <small>Interact with the page to re-connect.</small>
      </div>
    );
  }

  // All good. Don't render anything
  return (
    <>
    </>
  );
};
