import React from 'react';
import { globals } from '../singletons/globals.singleton';

export const AppFooter: React.FC = (props) => (
  <div className="app-footer">
    <p>
      <span>{`${globals.appTitle} v${globals.version}`}</span>
    </p>
    <p className="djpoida">
      <img src="img/dJP.svg" alt="dJP" />
      <span>by Peter Eldred (dJPoida)</span>
    </p>
    <p>
      <a href={globals.homepage} target="_blank" rel="noreferrer">{globals.homepage}</a>
    </p>
  </div>
);
