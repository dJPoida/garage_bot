import React, { createContext, useContext, useState } from "react";
import { A_DOOR_STATE, DOOR_STATE } from "../constants/door-state.const";
import { SocketClientContext } from "./socket-client.provider";

type DoorControlContextProps = {
  doorState: A_DOOR_STATE;
};

export const DoorControlContext = createContext<DoorControlContextProps>(
  null as any
);

export const DoorControlContextConsumer = DoorControlContext.Consumer;

/**
 * a uniform way of delivering props to a component that is dependent on the doorControl
 */
export const DoorControlProvider: React.FC = ({ children }) => {
  const [doorState, setDoorState] = useState<A_DOOR_STATE>(DOOR_STATE.UNKNOWN);

  const { socketClientState } = useContext(SocketClientContext);

  // Render
  return (
    <DoorControlContext.Provider
      value={{
        doorState,
      }}
    >
      {children}
    </DoorControlContext.Provider>
  );
};
