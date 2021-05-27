import React, { useContext, useState } from "react";
import {
  A_CONTROL_PAGE,
  ControlPageComponentMap,
  CONTROL_PAGE,
} from "../../constants/control-page.const";
import { SOCKET_CLIENT_STATE } from "../../constants/socket-client-state.const";
import { DeviceContext } from "../../providers/device.provider";
import { AppHeader } from "../app-header";
import { AppMenu } from "../app-menu";

export const ControlApp: React.FC = () => {
  const { socketClientState } = useContext(DeviceContext);

  const [currentPage, setCurrentPage] = useState<A_CONTROL_PAGE>(
    CONTROL_PAGE.CONTROL
  );

  const CurrentPageComponent = ControlPageComponentMap[currentPage];

  return (
    <div className="app control">
      <AppMenu
        pages={[
          CONTROL_PAGE.CONTROL,
          CONTROL_PAGE.CONFIG,
          CONTROL_PAGE.CALIBRATION,
          CONTROL_PAGE.ABOUT,
        ]}
        currentPage={currentPage}
        onChangePage={setCurrentPage}
      />
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

      {/* Connected - render the appropriate page */}
      {socketClientState === SOCKET_CLIENT_STATE.CONNECTED && (
        <CurrentPageComponent />
      )}
    </div>
  );
};
