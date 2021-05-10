import React from "react";
import { globals } from "../helpers/globals.helper";

export const AppFooter: React.FC = (props) => {
  return (
    <div className="footer">
      <p>
        <span>
          {globals.appTitle} v{globals.version}
        </span>
        <br />
        <span>by Peter Eldred (dJPoida)</span>
        <br />
        <a href={globals.homepage}>{globals.homepage}</a>
      </p>
    </div>
  );
};
