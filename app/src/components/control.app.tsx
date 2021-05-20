import React, { useContext } from "react";
import { SOCKET_CLIENT_STATE } from "../constants/socket-client-state.const";
import { SocketClientContext } from "../providers/socket-client.provider";
import { AppHeader } from "./app-header";

export const ControlApp: React.FC = () => {
  const { state } = useContext(SocketClientContext);

  return (
    <div className="app control">
      <AppHeader />

      {/* Connecting to the device */}
      {state === SOCKET_CLIENT_STATE.CONNECTING && (
        <div className="please-wait">
          <span className="spinner" />
          <span>Connecting to device...</span>
        </div>
      )}

      {/* Connected */}
      {state === SOCKET_CLIENT_STATE.CONNECTED && (
        <div>
          <span>Connected.</span>
        </div>
      )}
    </div>
  );
};
