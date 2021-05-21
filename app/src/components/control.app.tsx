import React, { useContext } from "react";
import { SOCKET_CLIENT_STATE } from "../constants/socket-client-state.const";
import { SocketClientContext } from "../providers/socket-client.provider";
import { AppHeader } from "./app-header";

export const ControlApp: React.FC = () => {
  const { socketClientState } = useContext(SocketClientContext);

  return (
    <div className="app control">
      <AppHeader />

      {/* Connecting to the device */}
      {socketClientState === SOCKET_CLIENT_STATE.CONNECTING && (
        <div className="please-wait">
          <span className="spinner" />
          <span>Connecting to device...</span>
        </div>
      )}

      {/* Disconnected */}
      {socketClientState === SOCKET_CLIENT_STATE.DISCONNECTED && (
        <div className="disconnected">
          <span className="icon-alert"></span>
          <span>Disconnected!</span>
        </div>
      )}

      {/* Connected */}
      {socketClientState === SOCKET_CLIENT_STATE.CONNECTED && (
        <div>
          <span>Connected.</span>
        </div>
      )}
    </div>
  );
};
