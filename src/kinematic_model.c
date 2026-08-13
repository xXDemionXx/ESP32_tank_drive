/**
 * This file contains the implementation of the kinetic model 
 * that is called in every kinematics task loop and it returns
 * the speeds of the motors.
 */
#include "kinematic_model.h"
#include <stdlib.h>

// Private macros
#define MAX_SPEED 10000
#define SPEED_RANGE MAX_SPEED * 2

void kinematic_model(tank_drive_move_command_t in, tank_drive_motor_speeds_t *speeds)
{
    int16_t T_middle_speed = in.T * MAX_SPEED;

    if (in.R == 1.0f || in.R == -1.0f) // Spinning in place
    {
        speeds->left = T_middle_speed * in.R;
        speeds->right = -T_middle_speed * in.R;
    }
    else // All the other movement
    {
        int16_t R_speed_range = in.R * SPEED_RANGE;
        uint16_t R_abs_delta_half = abs(R_speed_range) / 2;
        //
        if (T_middle_speed + R_abs_delta_half > MAX_SPEED)
        {
            // Limit translational speed when otherwise the rotation would't be possible
            T_middle_speed = MAX_SPEED - R_abs_delta_half;
        }
        else if (T_middle_speed - R_abs_delta_half < -MAX_SPEED)
        {
            // Limit translational speed when otherwise the rotation would't be possible
            T_middle_speed = -MAX_SPEED + R_abs_delta_half;
        }
        else if (abs(T_middle_speed) - R_abs_delta_half < 0)
        {
            // Limoit rotation when there isn't enough translational speed
            R_speed_range = abs(T_middle_speed) * 2;
            if (in.R < 0)
                R_speed_range *= -1;
        }
        //
        speeds->left = T_middle_speed + R_speed_range / 2;
        speeds->right = T_middle_speed - R_speed_range / 2;
    }
}
