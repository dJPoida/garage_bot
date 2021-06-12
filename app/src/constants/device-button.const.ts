/**
 * These are the virtual buttons that perform functions on the device. These don't
 * correlate with physical buttons in any way.
 */
export const VIRTUAL_BUTTON = {
  // Make sure these match the values defined on the device firmware in `helpers.h`
  ACTIVATE: 'ACTIVATE',
  OPEN: 'OPEN',
  CLOSE: 'CLOSE',
} as const;
export type VIRTUAL_BUTTON = typeof VIRTUAL_BUTTON;
export type A_VIRTUAL_BUTTON = VIRTUAL_BUTTON[keyof VIRTUAL_BUTTON];

export const DeviceButtonDescription: { [K in A_VIRTUAL_BUTTON]: string } = {
  [VIRTUAL_BUTTON.ACTIVATE]: 'Activate',
  [VIRTUAL_BUTTON.OPEN]: 'Open',
  [VIRTUAL_BUTTON.CLOSE]: 'Close',
};
