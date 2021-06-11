import React, { useContext, useEffect, useState } from "react";
import {
  BrowserRouter as Router,
  Switch,
  Route,
  useLocation,
} from "react-router-dom";
import {
  A_CONTROL_PAGE,
  ControlPageConfig,
  CONTROL_PAGE,
} from "../../constants/control-page.const";
import { SOCKET_CLIENT_STATE } from "../../constants/socket-client-state.const";
import { DeviceContext } from "../../providers/device.provider";
import { AppHeader } from "../app-header";
import { AppMenu } from "../app-menu";

export const ControlApp: React.FC = () => {
  const { socketClientState } = useContext(DeviceContext);

  const location = useLocation();
  const [currentPage, setCurrentPage] = useState<A_CONTROL_PAGE>(
    CONTROL_PAGE.CONTROL
  );

  /**
   * Evaluate the current location.pathname and convert it to a pageKey
   */
  useEffect(() => {
    const foundPage: null | A_CONTROL_PAGE = Object.keys(
      ControlPageConfig
    ).find(
      (controlPageKey) =>
        ControlPageConfig[controlPageKey as A_CONTROL_PAGE].route ===
        location.pathname
    ) as null | A_CONTROL_PAGE;
    if (foundPage) {
      setCurrentPage(foundPage ?? CONTROL_PAGE.CONTROL);
    }
  }, [location]);

  // Render
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
      />

      {/* AppHeader */}
      <AppHeader />

      {/* Connecting to the device */}
      {socketClientState === SOCKET_CLIENT_STATE.CONNECTING && (
        <div className="connection please-wait">
          <span className="spinner" />
          <span>Connecting to device...</span>
        </div>
      )}

      {/* Disconnected */}
      {socketClientState === SOCKET_CLIENT_STATE.DISCONNECTED && (
        <div className="connection disconnected">
          <span className="icon-alert"></span>
          <span>Disconnected!</span>
        </div>
      )}

      {/* Connected - render the appropriate page */}
      {socketClientState === SOCKET_CLIENT_STATE.CONNECTED && (
        <Switch>
          {Object.values(CONTROL_PAGE).map((controlPageKey) => {
            const config = ControlPageConfig[controlPageKey];
            return (
              <Route
                exact
                key={controlPageKey}
                path={config.route}
                component={config.pageComponent}
              />
            );
          })}
        </Switch>
      )}
    </div>
  );
};
