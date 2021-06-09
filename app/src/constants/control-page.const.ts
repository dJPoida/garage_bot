import React from "react";

import { AboutPage } from "../components/pages/about.page";
import { CalibrationPage } from "../components/pages/calibration.page";
import { ConfigPage } from "../components/pages/config.page";
import { ControlPage } from "../components/pages/control.page";

import { AN_ICON, ICON } from "./icon.const";

/**
 * These are the pages that the user can display on the control page
 */
export const CONTROL_PAGE = {
  CONTROL: "CONTROL",
  CONFIG: "CONFIG",
  CALIBRATION: "CALIBRATION",
  ABOUT: "ABOUT",
} as const;
export type CONTROL_PAGE = typeof CONTROL_PAGE;
export type A_CONTROL_PAGE = CONTROL_PAGE[keyof CONTROL_PAGE];

interface IControlPageConfig {
  title: string;
  icon: AN_ICON;
  route: string;
  pageComponent: React.FC;
}

/**
 * These are the page control details
 */
export const ControlPageConfig: { [K in A_CONTROL_PAGE]: IControlPageConfig } =
  {
    [CONTROL_PAGE.CONTROL]: {
      title: "Control",
      icon: ICON.RSS,
      route: "/",
      pageComponent: ControlPage,
    },
    [CONTROL_PAGE.CONFIG]: {
      title: "Config",
      icon: ICON.SETTINGS,
      route: "/config",
      pageComponent: ConfigPage,
    },
    [CONTROL_PAGE.CALIBRATION]: {
      title: "Calibration",
      icon: ICON.EDIT,
      route: "/calibration",
      pageComponent: CalibrationPage,
    },
    [CONTROL_PAGE.ABOUT]: {
      title: "About",
      icon: ICON.INFO,
      route: "/about",
      pageComponent: AboutPage,
    },
  };
