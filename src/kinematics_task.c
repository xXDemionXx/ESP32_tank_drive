#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "kinematics_task.h"
#include "tank_move_command.h"
#include "motor_control.h"
#include "drive_hardware_definitions.h"
#include "esp_log.h"

// Private defines
#define KINEMATICS_TASK_PERIOD_MS 10
#define MOTOR_MAX_POSITIVE_SPEED 10000
#define MOTOR_MAX_NEGATIVE_SPEED -10000

// Private constants
static const char *TAG = "kinematics_task";

// Private function prototypes

static void kinematic_model(float T, float R, int16_t *L_speed, int16_t *R_speed);

// Public functions

void kinematics_task(void *p_param)
{
    //
    move_command_getter getter = (move_command_getter)p_param;
    tank_drive_move_command_t command = {};
    struct
    {
        int16_t L;
        int16_t R;
    } motor_speeds;
    // Create motors
    motor_handle_t L_motor = motor_init(L_IN_A_PIN, L_IN_B_PIN, L_MODE_PIN, L_SLEEP_PIN);
    motor_handle_t R_motor = motor_init(R_IN_A_PIN, R_IN_B_PIN, R_MODE_PIN, R_SLEEP_PIN);
//
#ifdef CONFIG_TANK_DRIVE_LEFT_MOTOR_INVERT
    motor_reverse_drive_directions(L_motor);
#endif
#ifdef CONFIG_TANK_DRIVE_RIGHT_MOTOR_INVERT
    motor_reverse_drive_directions(R_motor);
#endif
    //
    TickType_t xLastWakeTime = xTaskGetTickCount();
    //
    for (;;)
    {
        command = getter();
        //
        // #ifdef CONFIG_TANK_DRIVE_INVERT_FORWARD_BACKWARD
        //         command.T *= -1;
        // #endif
        // #ifdef CONFIG_TANK_DRIVE_INVERT_ROTATION
        //         command.R *= -1;
        // #endif
        //
        kinematic_model(command.T, command.R, &motor_speeds.L, &motor_speeds.R);
        //
        // Set motor speeds
        ESP_LOGI(TAG, "\nT: %1.3f, R: %1.3f \nL_speed: %5d, R_speed: %5d \n", command.T, command.R, motor_speeds.L, motor_speeds.R);
        //
#ifdef CONFIG_TANK_DRIVE_INVERT_FORWARD_BACKWARD
        motor_speeds.L *= -1;
        motor_speeds.R *= -1;
#endif
#ifdef CONFIG_TANK_DRIVE_INVERT_ROTATION
        // XOR swap variables
        motor_speeds.L = motor_speeds.L ^ motor_speeds.R;
        motor_speeds.R = motor_speeds.L ^ motor_speeds.R;
        motor_speeds.L = motor_speeds.L ^ motor_speeds.R;
#endif
        //
        motor_set_speed(L_motor, motor_speeds.L);
        motor_set_speed(R_motor, motor_speeds.R);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(KINEMATICS_TASK_PERIOD_MS));
    }
}

static void kinematic_model(float T, float R, int16_t *L_speed, int16_t *R_speed)
{
    int16_t T_middle_speed = T * MOTOR_MAX_POSITIVE_SPEED;

    if (R == 1.0f || R == -1.0f) // Spinning in place
    {
        *L_speed = T_middle_speed * R;
        *R_speed = -T_middle_speed * R;
    }
    else // All the other movement
    {
        int16_t R_speed_range = R * (MOTOR_MAX_POSITIVE_SPEED - MOTOR_MAX_NEGATIVE_SPEED);
        uint16_t R_abs_delta_half = abs(R_speed_range) / 2;
        //
        if (T_middle_speed + R_abs_delta_half > MOTOR_MAX_POSITIVE_SPEED)
        {
            // Limit translational speed when otherwise the rotation would't be possible
            T_middle_speed = MOTOR_MAX_POSITIVE_SPEED - R_abs_delta_half;
        }
        else if (T_middle_speed - R_abs_delta_half < MOTOR_MAX_NEGATIVE_SPEED)
        {
            // Limit translational speed when otherwise the rotation would't be possible
            T_middle_speed = MOTOR_MAX_NEGATIVE_SPEED + R_abs_delta_half;
        }
        else if (abs(T_middle_speed) - R_abs_delta_half < 0)
        {
            // Limoit rotation when there isn't enough translational speed
            R_speed_range = abs(T_middle_speed) * 2;
            if (R < 0)
                R_speed_range *= -1;
        }
        //
        //
        //
        if (T >= 0.0f)  // Forwards direction
        {
            *L_speed = T_middle_speed + R_speed_range / 2;
            *R_speed = T_middle_speed - R_speed_range / 2;
        }
        else    // Backwards direction
        {
            *L_speed = T_middle_speed - R_speed_range / 2;
            *R_speed = T_middle_speed + R_speed_range / 2;
        }
    }
}
