import React, { createRef, useCallback, useState } from "react";
import {
  A_CONTROL_PAGE,
  ControlPageIcon,
  ControlPageTitle,
} from "../constants/control-page.const";
import { globals } from "../helpers/globals.helper";
import { AppIcon } from "./app-icon";

export type AppMenuProps = {
  pages: A_CONTROL_PAGE[];
  currentPage: A_CONTROL_PAGE;
  onChangePage: (newPage: A_CONTROL_PAGE) => void;
};

export const AppMenu: React.FC<AppMenuProps> = (props) => {
  const { pages, currentPage, onChangePage } = props;
  const [visible, setVisible] = useState<boolean>(false);

  /**
   * Fired when one of the buttons is clicked
   */
  const handleButtonClick = useCallback((page: A_CONTROL_PAGE) => {
    // Call the page change function
    onChangePage(page);

    // Close the menu drawer
    setVisible(false);
  }, []);

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
          {pages.map((page) => (
            <button
              key={page}
              className={page === currentPage ? "primary" : ""}
              onClick={() => handleButtonClick(page)}
            >
              <span className={ControlPageIcon[page]}></span>
              <span>{ControlPageTitle[page]}</span>
            </button>
          ))}
        </div>
      </div>
    </div>
  );
};
