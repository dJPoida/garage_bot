import React from 'react';
import { globals } from '../singletons/globals.singleton';
import { AppIcon } from './app-icon';

export const AppTitle: React.FC = () => (
  <div className="app-title">
    <AppIcon />
    <div className="headers">
      <h1>{globals.appTitle}</h1>
      <h6>{`v${globals.version} (${globals.firmwareVersion})`}</h6>
    </div>
  </div>
);
