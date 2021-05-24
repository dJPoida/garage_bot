import * as React from "react";
import * as ReactDOM from "react-dom";
import { ControlApp } from "./components/control.app";
import { DeviceProvider } from "./providers/device.provider";

import "./scss/control.scss";

ReactDOM.render(
  <DeviceProvider>
    <ControlApp />
  </DeviceProvider>,
  document.getElementById("app")
);
