/**
 * This file describes how a kinematics function should look.
 * It also describes the ralation between input variables and
 * real movement of the tank.
 */
#ifndef KINEMATIC_MODEL_H
#define KINEMATIC_MODEL_H

#include "tank_move_command.h"
#include <stdint.h>

/**
 * @brief Tank drive motor speeds.
 */
typedef struct{
    int16_t left;   /**< Left motor speed (-10000 to 10000 corresponding to -100.00% to 100.00%) */
    int16_t right;  /**< Right motor speed (-10000 to 10000 corresponding to -100.00% to 100.00%) */
}tank_drive_motor_speeds_t;

/**
 * @brief Converts a tank drive command into left and right motor speeds.
 *
 * The kinematic model takes normalized translational and rotational movement
 * coefficients and computes the corresponding motor speeds.
 *
 * The translational coefficient (`in.T`) specifies the desired forward or
 * backward movement:
 * - `1.0` = maximum forward speed
 * - `0.0` = no translational movement
 * - `-1.0` = maximum backward speed
 *
 * The rotational coefficient (`in.R`) specifies the desired rotation:
 * - `1.0` = maximum clockwise rotation
 * - `0.0` = no rotational movement
 * - `-1.0` = maximum counterclockwise rotation
 *
 * When `in.T` is zero and `in.R` is non-zero, the tank rotates in place.
 *
 * @param[in] in
 *      Tank drive movement command. Both `T` and `R` are normalized values in
 *      the range [-1.0, 1.0].
 * @param[out] speeds
 *      Pointer to the structure that receives the calculated motor speeds.
 */
void kinematic_model(tank_drive_move_command_t in, tank_drive_motor_speeds_t *speeds);

#endif // KINEMATIC_MODEL_H
