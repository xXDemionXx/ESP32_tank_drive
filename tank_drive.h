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
 * Set the pointer to the variable that will be used as the deadman trigger.
 * The deadman is considdered pressed if it's value isn't 0. After the deadmans
 * value is zero for milliseconds_delay time the tank drive will stop it's movement.
 * Tank drive will resume it's movements once the deadman is held again for 
 * milliseconds_delay time.
 * 
 * @param deadman Pointer to the variable used as deadman.
 * @param milliseconds_delay Delay between the deadman being released and the tank stopping.
 */
void TankDriveSetDeadman(char *deadman, const uint16_t milliseconds_delay);

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
