import React, { useContext } from "react";
import { DoorStateDescriptionMap } from "../../constants/door-state.const";
import { DeviceContext } from "../../providers/device.provider";

export const ControlPage: React.FC = () => {
  const { doorState } = useContext(DeviceContext);

  return (
    <div>
      TODO: control page
      <div>
        <span>Connected.</span>
      </div>
      <div>
        <span>{DoorStateDescriptionMap[doorState]}</span>
      </div>
    </div>
  );
};
