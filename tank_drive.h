#ifndef TANK_DRIVE_H
#define TANK_DRIVE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "tank_move_command.h"

// Public functions

void TankDriveInit(move_command_getter);

/**
 * @brief Set up deadman trigger feature.
 * 
 * Set the function that will be used for getting the value of the deadman trigger.
 * The deadman is considdered pressed if it's value isn't 0. After the deadmans
 * value is zero for ms_release_delay time the tank drive will stop it's movement.
 * Tank drive will resume it's movements once the deadman is held again for 
 * ms_hold_delay time.
 * 
 * @param deadman_getter Pointer to the function used for getting the deadman's value.
 * @param ms_hold_delay Minimum time that the deadman needs to be pressed to be considered held.
 * @param ms_release_delay Minimum time that the deadman needs to be unpressed to be considered released.
 * @note Due to the way deadman was implemented, exact hold and release times can't be specified.
 *       It deppends on the kinematic task period, so the delays are minimum times.
 */
void TankDriveSetDeadman(char (*deadman_getter)(void), uint16_t ms_hold_delay, uint16_t ms_release_delay);

/**
 * @brief Set up emergency stop feature.
 * 
 * If the emergency stop value becomes anything but 0,
 * the tank stops. There is no way of ressuming, the tank must
 * be turned on and off again.
 * 
 * @param emergency_stop Pointer to the variable used as emergency stop.
 */
void TankDriveSetEmergencyStop(char *emergency_stop);

#ifdef __cplusplus
}
#endif

#endif // TANK_DRIVE_H
