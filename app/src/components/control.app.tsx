import React, { useContext } from "react";
import { DoorStateDescriptionMap } from "../constants/door-state.const";
import { SOCKET_CLIENT_STATE } from "../constants/socket-client-state.const";
import { DeviceContext } from "../providers/device.provider";
import { AppHeader } from "./app-header";

export const ControlApp: React.FC = () => {
  const { doorState, socketClientState, config } = useContext(DeviceContext);

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
        <>
          <div>
            <span>Connected.</span>
          </div>
          <div>
            <span>{DoorStateDescriptionMap[doorState]}</span>
          </div>
          <hr />
          <div className="config-list">
            {Object.keys(config).map((configKey: string) => {
              return (
                <div className="item" key={configKey}>
                  <div className="key">{`${configKey}:`}</div>
                  <div className="value">{config[configKey] as string}</div>
                </div>
              );
            })}
          </div>
        </>
      )}
    </div>
  );
};
