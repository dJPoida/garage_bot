import React from "react";
import { globals } from "../helpers/globals.helper";
import { AppIcon } from "./app-icon";

export const AppHeader: React.FC = (props) => {
  return (
    <div className="app-header">
      <AppIcon />
      <h1>{globals.appTitle}</h1>
      <h6>{`v${globals.version} (${globals.firmwareVersion})`}</h6>
    </div>
  );
};
