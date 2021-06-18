import React, { useContext } from 'react';
import classNames from 'classnames';

import { DeviceContext } from '../providers/device.provider';

import { A_DOOR_STATE, DOOR_STATE } from '../constants/door-state.const';

export const DoorStatusIndicators: React.FC = () => {
  const { doorState } = useContext(DeviceContext);

  return (
    <div className="door-status-indicators">
      <div
        className={classNames('state-indicator', 'open', {
          active: doorState === DOOR_STATE.OPEN,
        })}
      >
        {doorState === DOOR_STATE.OPEN && <span>OPEN</span>}
      </div>
      <div
        className={classNames('state-indicator', 'transitioning', {
          active: (
            [DOOR_STATE.UNKNOWN, DOOR_STATE.OPENING, DOOR_STATE.CLOSING] as A_DOOR_STATE[]
          ).includes(doorState),
        })}
      >
        {doorState === DOOR_STATE.OPENING && <span>OPENING</span>}
        {doorState === DOOR_STATE.CLOSING && <span>CLOSING</span>}
        {doorState === DOOR_STATE.UNKNOWN && <span>UNKNOWN</span>}
      </div>
      <div
        className={classNames('state-indicator', 'closed', {
          active: doorState === DOOR_STATE.CLOSED,
        })}
      >
        {doorState === DOOR_STATE.CLOSED && <span>CLOSED</span>}
      </div>
    </div>
  );
};
