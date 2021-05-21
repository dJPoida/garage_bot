import React from 'react';
import { A_DOOR_STATE } from '../constants/door-state.const';

export type UpDownButtonsProps = {
  doorState: A_DOOR_STATE,
}

export const UpDownButtons: React.FC<UpDownButtonsProps> = (props)=> {
  return (
    <div className="up-down-buttons">
      <button>UP</button>
      <button>PRESS</button>
      <button>DOWN</button>
    </div>
  );
}