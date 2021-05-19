import React, { useEffect, useState } from "react";
import { socketClient } from "../helpers/socket-client.helper";
import { AppHeader } from "./app-header";

export const ControlApp: React.FC = () => {
  const [connected, setConnected] = useState(socketClient.connected);

  return (
    <div className="app control">
      <AppHeader />

      {!connected && (
        <div className="please-wait">
          <span className="spinner" />
          <span>Connecting to device...</span>
        </div>
      )}
    </div>
  );
};
