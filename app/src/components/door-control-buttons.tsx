import React, { useContext } from 'react';

import { DeviceContext } from '../providers/device.provider';

import { A_DOOR_STATE, DOOR_STATE } from '../constants/door-state.const';
import { ICON } from '../constants/icon.const';
import { VIRTUAL_BUTTON } from '../constants/device-button.const';

export const DoorControlButtons:React.FC = () => {
  const { doorState, pressButton } = useContext(DeviceContext);

  return (
    <div className="door-control-buttons">
      {/* Up Button */}
      <button
        type="button"
        onClick={() => pressButton(VIRTUAL_BUTTON.OPEN)}
        disabled={([DOOR_STATE.UNKNOWN, DOOR_STATE.OPEN, DOOR_STATE.OPENING] as A_DOOR_STATE[]).includes(doorState)}
      >
        <span className={ICON.UP} />
      </button>

      {/* Activate Button */}
      <button
        type="button"
        onClick={() => pressButton(VIRTUAL_BUTTON.ACTIVATE)}
        disabled={([DOOR_STATE.UNKNOWN] as A_DOOR_STATE[]).includes(doorState)}
      >
        <span className={ICON.TOGGLE} />
      </button>

      {/* Down Button */}
      <button
        type="button"
        onClick={() => pressButton(VIRTUAL_BUTTON.CLOSE)}
        disabled={([DOOR_STATE.UNKNOWN, DOOR_STATE.CLOSED, DOOR_STATE.CLOSING] as A_DOOR_STATE[]).includes(doorState)}
      >
        <span className={ICON.DOWN} />
      </button>
    </div>
  );
};
