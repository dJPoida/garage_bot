import React, { createRef, useCallback, useEffect, useState } from "react";
import { Link, NavLink } from "react-router-dom";
import {
  A_CONTROL_PAGE,
  ControlPageConfig,
} from "../constants/control-page.const";
import { globals } from "../helpers/globals.helper";
import { AppIcon } from "./app-icon";

export type AppMenuProps = {
  pages: A_CONTROL_PAGE[];
  currentPage: A_CONTROL_PAGE;
};

export const AppMenu: React.FC<AppMenuProps> = (props) => {
  const { pages, currentPage } = props;
  const [visible, setVisible] = useState<boolean>(false);

  /**
   * When the location changes, hide the nav
   */
  useEffect(() => {
    setVisible(false);
  }, [currentPage]);

  // Render
  return (
    <div className="app-menu">
      <label htmlFor="drawer-control" className="drawer-toggle persistent" />
      <input
        type="checkbox"
        id="drawer-control"
        className="drawer persistent"
        checked={visible}
        onChange={(e) => {
          setVisible(e.target.checked);
        }}
      />
      <div className="drawer-wrapper">
        <div className="drawer">
          <label htmlFor="drawer-control" className="drawer-close"></label>
          <div className="app-title">
            <AppIcon />
            <div className="title-wrapper">
              <span>{globals.appTitle}</span>
              <span>{`v${globals.version} (${globals.firmwareVersion})`}</span>
            </div>
          </div>
          <div className="button-wrapper">
            {pages.map((pageKey) => {
              const config = ControlPageConfig[pageKey];
              return (
                <NavLink
                  exact
                  key={pageKey}
                  className="button"
                  to={config.route}
                  activeClassName="primary"
                >
                  {config.title}
                </NavLink>
              );
            })}
          </div>
        </div>
      </div>
    </div>
  );
};
