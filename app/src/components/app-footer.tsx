import React from 'react';
import { globals } from '../singletons/globals.singleton';

export const AppFooter: React.FC = (props) => (
  <div className="footer">
    <p>
      <span>{`${globals.appTitle} v${globals.version}`}</span>
      <br />
      <span>by Peter Eldred (dJPoida)</span>
      <br />
      <a href={globals.homepage}>{globals.homepage}</a>
    </p>
  </div>
);
