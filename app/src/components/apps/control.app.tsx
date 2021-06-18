import React, { useContext, useEffect, useState } from 'react';
import {
  Switch,
  Route,
  useLocation,
  useHistory,
} from 'react-router-dom';

import { DeviceContext } from '../../providers/device.provider';

import { AppMenu } from '../app-menu';

import { SOCKET_CLIENT_STATE } from '../../constants/socket-client-state.const';
import { A_CONTROL_PAGE, ControlPageConfig, CONTROL_PAGE } from '../../constants/control-page.const';
import { usePreviousValue } from '../../react-hooks/use-previous-value.hook';
import { ConnectionStatus } from '../connection-status';

export const ControlApp: React.FC = () => {
  const { rebooting, socketClientState } = useContext(DeviceContext);

  const history = useHistory();
  const location = useLocation();
  const [currentPage, setCurrentPage] = useState<A_CONTROL_PAGE>(CONTROL_PAGE.CONTROL);

  const oldRebooting = usePreviousValue(rebooting);

  /**
   * After the device has finished rebooting, switch back to the control page
   */
  useEffect(() => {
    if ((oldRebooting !== rebooting) && !rebooting) {
      history.push('/');
    }
  }, [rebooting, oldRebooting, history]);


  /**
   * Evaluate the current location.pathname and convert it to a pageKey
   */
  useEffect(() => {
    const foundPage: null | A_CONTROL_PAGE = Object.keys(
      ControlPageConfig,
    ).find(
      (controlPageKey) => ControlPageConfig[controlPageKey as A_CONTROL_PAGE].route
        === location.pathname,
    ) as null | A_CONTROL_PAGE;
    if (foundPage) {
      setCurrentPage(foundPage ?? CONTROL_PAGE.CONTROL);
    }
  }, [location]);


  // Render
  return (
    <div className="app control">
      <ConnectionStatus />

      {/* Connected - render the appropriate page */}
      {!rebooting && (socketClientState === SOCKET_CLIENT_STATE.CONNECTED) && (
        <>
          <AppMenu
            pages={[
              CONTROL_PAGE.CONTROL,
              CONTROL_PAGE.CONFIG,
              CONTROL_PAGE.CALIBRATION,
              CONTROL_PAGE.ABOUT,
            ]}
            currentPage={currentPage}
          />

          <Switch>
            {Object.values(CONTROL_PAGE).map((controlPageKey) => {
              const config = ControlPageConfig[controlPageKey];
              const PageComponent = config.pageComponent;
              return (
                <Route
                  exact
                  key={controlPageKey}
                  path={config.route}
                >
                  <PageComponent
                    {...config}
                  />
                </Route>
              );
            })}
          </Switch>
        </>
      )}


    </div>
  );
};
