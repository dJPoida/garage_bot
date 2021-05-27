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

/**
 * These are the page titles for each of the control pages
 */
export const ControlPageTitle: { [K in A_CONTROL_PAGE]: string } = {
  [CONTROL_PAGE.CONTROL]: "Control",
  [CONTROL_PAGE.CONFIG]: "Config",
  [CONTROL_PAGE.CALIBRATION]: "Calibration",
  [CONTROL_PAGE.ABOUT]: "About",
};

/**
 * These are the icons for each of the control pages
 */
export const ControlPageIcon: { [K in A_CONTROL_PAGE]: AN_ICON } = {
  [CONTROL_PAGE.CONTROL]: ICON.RSS,
  [CONTROL_PAGE.CONFIG]: ICON.SETTINGS,
  [CONTROL_PAGE.CALIBRATION]: ICON.EDIT,
  [CONTROL_PAGE.ABOUT]: ICON.INFO,
};

/**
 * These are the page components to render for each of the pages
 */
export const ControlPageComponentMap: { [K in A_CONTROL_PAGE]: React.FC } = {
  [CONTROL_PAGE.CONTROL]: ControlPage,
  [CONTROL_PAGE.CONFIG]: ConfigPage,
  [CONTROL_PAGE.CALIBRATION]: CalibrationPage,
  [CONTROL_PAGE.ABOUT]: AboutPage,
};
