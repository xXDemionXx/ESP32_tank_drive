#include "tank_drive.h"
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "kinematic_task.h"
#include "tank_drive_settings.h"
#include "esp_log.h"

// Private constants

static const char *TAG = "tank_drive";

// Private variables

tank_drive_settings_t _tank_drive_settings;

// Public functions

void TankDriveInit(tank_drive_move_command_getter getter)
{
    static bool init_already_completed = false;
    if (init_already_completed == true) // Do not allow for multiple init calls
        ESP_LOGE(TAG, "Multiple calls for init");
    //
    if (pdTRUE != xTaskCreate(kinematic_task,
                              KINEMATICS_TASK_NAME,
                              KINEMATICS_TASK_SIZE,
                              (void *)getter,
                              KINEMATICS_TASK_PRIORITY,
                              NULL))
    {
        ESP_LOGE(TAG, "init failed");
    }
    else
    {
        init_already_completed = true;
    }
}

void TankDriveSetDeadman(char (*deadman_getter)(void), uint16_t ms_hold_delay, uint16_t ms_release_delay)
{
    if (!deadman_getter)
        ESP_LOGE(TAG, "NULL deadman getter");

    _tank_drive_settings.deadman.getter = deadman_getter;
    _tank_drive_settings.deadman.ms_hold_delay = ms_hold_delay;
    _tank_drive_settings.deadman.ms_release_delay = ms_release_delay;
    _tank_drive_settings.deadman.enabled = true;
}

void TankDriveSetEmergencyStop(char (*emergency_stop_getter)(void))
{
    if (!emergency_stop_getter)
        ESP_LOGE(TAG, "NULL emergency stop getter");

    _tank_drive_settings.emergency_stop.getter = emergency_stop_getter;
    _tank_drive_settings.emergency_stop.enabled = true;
}
