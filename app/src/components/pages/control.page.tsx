import React, { useCallback, useContext } from "react";
import classNames from "classnames";
import {
  A_DOOR_STATE,
  DoorStateDescriptionMap,
  DOOR_STATE,
} from "../../constants/door-state.const";
import { ICON } from "../../constants/icon.const";
import { DeviceContext } from "../../providers/device.provider";

export const ControlPage: React.FC = () => {
  const { doorState, pressButton, releaseButton } = useContext(DeviceContext);

  return (
    <div>
      <div>
        <span>Connected.</span>
      </div>
      <div>
        <span>{DoorStateDescriptionMap[doorState]}</span>
      </div>
      <div className="control-wrapper">
        <div className="control-buttons">
          <button
            disabled={([
              DOOR_STATE.OPEN,
              DOOR_STATE.OPENING,
            ] as A_DOOR_STATE[]).includes(doorState)}
          >
            <span className={ICON.UP} />
          </button>
          <button onMouseDown={pressButton} onMouseUp={releaseButton}>
            <span className={ICON.TOGGLE} />
          </button>
          <button
            disabled={([
              DOOR_STATE.CLOSED,
              DOOR_STATE.CLOSING,
            ] as A_DOOR_STATE[]).includes(doorState)}
          >
            <span className={ICON.DOWN} />
          </button>
        </div>
        <div className="status-indicators">
          <div className="open">
            <div
              className={classNames("state-indicator", {
                active: doorState === DOOR_STATE.OPEN,
              })}
            >
              {doorState === DOOR_STATE.OPEN && <span>OPEN</span>}
            </div>
          </div>
          <div className="transitioning">
            <div
              className={classNames("state-indicator", {
                active: ([
                  DOOR_STATE.OPENING,
                  DOOR_STATE.CLOSING,
                ] as A_DOOR_STATE[]).includes(doorState),
              })}
            >
              {doorState === DOOR_STATE.OPENING && <span>OPENING</span>}
              {doorState === DOOR_STATE.CLOSING && <span>CLOSING</span>}
            </div>
          </div>
          <div className="closed">
            <div
              className={classNames("state-indicator", {
                active: doorState === DOOR_STATE.CLOSED,
              })}
            >
              {doorState === DOOR_STATE.CLOSED && <span>CLOSED</span>}
            </div>
          </div>
        </div>
      </div>
    </div>
  );
};
