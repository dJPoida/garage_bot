import React from 'react';
import { globals } from '../singletons/globals.singleton';

export const AppIcon: React.FC = () => (
  <img alt={globals.appTitle} className="app-icon" src="img/appicon.svg" />
);
