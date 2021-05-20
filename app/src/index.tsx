import * as React from "react";
import * as ReactDOM from "react-dom";
import { ControlApp } from "./components/control.app";
import { SocketClientProvider } from "./providers/socket-client.provider";

import "./scss/control.scss";

ReactDOM.render(
  <SocketClientProvider>
    <ControlApp />
  </SocketClientProvider>,
  document.getElementById("app")
);
