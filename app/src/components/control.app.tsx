import React from "react";
import { globals } from "../helpers/globals.helper";
import { AppIcon } from "./app-icon";

export const ControlApp: React.FC = () => {
  return (
    <div className="app control">
      <AppIcon />
      <h1>{globals.appTitle}</h1>
      <h6>{globals.version}</h6>
    </div>
  );
};
