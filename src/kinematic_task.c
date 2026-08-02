#include "kinematic_task.h"
#include "kinematic_model.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tank_move_command.h"
#include "motor_control.h"
#include "drive_hardware_definitions.h"
#include "deadman.h"
#include "esp_log.h"
#include "tank_drive_settings.h"

// Private defines
#define KINEMATIC_TASK_PERIOD_MS CONFIG_TANK_DRIVE_TASK_LOOP_PERIOD_MS

// Private constants
static const char *TAG = "kinematic_task";

// Public functions
void kinematic_task(void *p_param)
{
    move_command_getter getter = (move_command_getter)p_param;
    tank_drive_move_command_t command = {};
    tank_drive_motor_speeds_t motor_speeds;
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
        if (_tank_drive_settings.emergency_stop.enabled == true && _tank_drive_settings.emergency_stop.getter() == 0)
        {
            // Turn off the motors
            motor_set_speed(L_motor, 0);
            motor_set_speed(R_motor, 0);
            // Susspend this task
            ESP_LOGI(TAG, "E-STOP pressed");
            vTaskSuspend(NULL);
        }
        else if (_tank_drive_settings.deadman.enabled == true && get_deadman_state() == false)
        {
            motor_set_speed(L_motor, 0);
            motor_set_speed(R_motor, 0);
            vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(KINEMATIC_TASK_PERIOD_MS));
        }
        else
        {
            command = getter();
            kinematic_model(command, &motor_speeds);
            //
#ifdef CONFIG_TANK_DRIVE_INVERT_FORWARD_BACKWARD
            motor_speeds.left *= -1;
            motor_speeds.right *= -1;
#endif
#ifdef CONFIG_TANK_DRIVE_INVERT_ROTATION
            // XOR swap variables
            motor_speeds.left = motor_speeds.left ^ motor_speeds.right;
            motor_speeds.right = motor_speeds.left ^ motor_speeds.right;
            motor_speeds.left = motor_speeds.left ^ motor_speeds.right;
#endif
            //
            motor_set_speed(L_motor, motor_speeds.left);
            motor_set_speed(R_motor, motor_speeds.right);
            vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(KINEMATIC_TASK_PERIOD_MS));
        }
    }
}
