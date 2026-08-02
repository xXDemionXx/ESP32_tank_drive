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

// Private function prototypes

// Public functions

void TankDriveInit(move_command_getter getter)
{
    if (pdTRUE != xTaskCreate(kinematic_task,
                              KINEMATICS_TASK_NAME,
                              KINEMATICS_TASK_SIZE,
                              (void *)getter,
                              //   getter,
                              //   NULL,
                              KINEMATICS_TASK_PRIORITY,
                              NULL))
    {
        ESP_LOGE(TAG, "init failed");
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

void TankDriveSetEmergencyStop(char *emergency_stop)
{
    if (!emergency_stop)
        ESP_LOGE(TAG, "NULL emergency stop");

    _tank_drive_settings.emergency_stop.value_p = emergency_stop;
    _tank_drive_settings.emergency_stop.enabled = true;
}
