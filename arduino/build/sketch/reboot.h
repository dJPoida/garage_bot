/*============================================================================*\
 * Garage Bot - reboot
 * Peter Eldred 2021-04
 * 
 * Simple wrapper for functions that help the device gracefully reboot
\*============================================================================*/

#ifndef REBOOT_H
#define REBOOT_H


/**
 * Flags the kernal down for a reboot
 */
void reboot();


/**
 * If flagged, reboots the device by pulling the reset pin high
 * Don't call this directly. Instead, use `reboot()` which
 * sets a flag that is checked on the next loop.
 */
void checkReboot();

#endif
