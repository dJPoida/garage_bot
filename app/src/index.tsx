import * as React from "react";
import * as ReactDOM from "react-dom";
import { ControlApp } from "./components/control.app";
import { DoorControlProvider } from "./providers/door-control.provider";
import { SocketClientProvider } from "./providers/socket-client.provider";

import "./scss/control.scss";

ReactDOM.render(
  <SocketClientProvider>
    <DoorControlProvider>
      <ControlApp />
    </DoorControlProvider>
  </SocketClientProvider>,
  document.getElementById("app")
);
