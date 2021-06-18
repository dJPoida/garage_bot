import React from 'react';

import { PageProps } from '../../types/page.props';

import { AppTitle } from '../app-title';
import { DoorControlButtons } from '../door-control-buttons';
import { DoorStatusIndicators } from '../door-status-indicators';

export const ControlPage: React.FC<PageProps> = () => (
  <div className="page control">
    <AppTitle />
    <div className="control-wrapper">
      <DoorStatusIndicators />
      <DoorControlButtons />
    </div>
  </div>
);
