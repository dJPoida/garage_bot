/**
 * These are the states that the garage door can be in.
 */
export const DOOR_STATE = {
  // Make sure these match the values defined on the device firmware in `helpers.h`
  UNKNOWN: 'UNKNOWN',
  OPEN: 'OPEN',
  CLOSING: 'CLOSING',
  CLOSED: 'CLOSED',
  OPENING: 'OPENING',
} as const;
export type DOOR_STATE = typeof DOOR_STATE;
export type A_DOOR_STATE = DOOR_STATE[keyof DOOR_STATE];

export const DoorStateDescriptionMap: { [K in A_DOOR_STATE]: string } = {
  [DOOR_STATE.UNKNOWN]: 'Unknown',
  [DOOR_STATE.OPEN]: 'Open',
  [DOOR_STATE.CLOSING]: 'Closing',
  [DOOR_STATE.CLOSED]: 'Closed',
  [DOOR_STATE.OPENING]: 'Opening',
};
