import * as React from "react";
import * as ReactDOM from "react-dom";
import { BrowserRouter as Router } from "react-router-dom";
import { ControlApp } from "./components/apps/control.app";
import { DeviceProvider } from "./providers/device.provider";

import "./scss/control.scss";

ReactDOM.render(
  <DeviceProvider>
    <Router>
      <ControlApp />
    </Router>
  </DeviceProvider>,
  document.getElementById("app")
);
