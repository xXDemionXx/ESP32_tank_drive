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
    TickType_t xLastWakeTime = xTaskGetTickCount();
    //
    for (;;)
    {
        command = getter();
        //
        kinematic_model(command.T, command.R, &motor_speeds.L, &motor_speeds.R);
        // Set motor speeds
        ESP_LOGI(TAG, "\nT: %1.1f, R: %1.1f \nL_speed: %5d, R_speed: %5d \n", command.T, command.R, motor_speeds.L, motor_speeds.R);
        motor_set_speed(L_motor, motor_speeds.L);
        motor_set_speed(R_motor, motor_speeds.R);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(KINEMATICS_TASK_PERIOD_MS));
    }
}

static void kinematic_model(float T, float R, int16_t *L_speed, int16_t *R_speed)
{
    int16_t T_middle_speed = T * MOTOR_MAX_POSITIVE_SPEED;
    int16_t R_speed_range = R * (MOTOR_MAX_POSITIVE_SPEED - MOTOR_MAX_NEGATIVE_SPEED);

    uint16_t R_abs_delta_half = abs(R_speed_range) / 2;

    if (T_middle_speed + R_abs_delta_half > MOTOR_MAX_POSITIVE_SPEED)
    {
        T_middle_speed = MOTOR_MAX_POSITIVE_SPEED - R_abs_delta_half;
    }
    else if (T_middle_speed - R_abs_delta_half < MOTOR_MAX_NEGATIVE_SPEED)
    {
        T_middle_speed = MOTOR_MAX_NEGATIVE_SPEED + R_abs_delta_half;
    }

    if (R == 1.0f || R == -1.0f) // Spinning in place
    {
        *L_speed = -T_middle_speed * R;
        *L_speed = T_middle_speed * R;
    }
    else // All the other movement
    {
        *L_speed = T_middle_speed - R_speed_range / 2;
        *R_speed = T_middle_speed + R_speed_range / 2;
    }
}
