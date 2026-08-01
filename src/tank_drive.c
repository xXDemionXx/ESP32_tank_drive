#include "tank_drive.h"
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "kinematic_task.h"
#include "esp_log.h"

// Private constants

static const char *TAG = "tank_drive";

// Private variables

static struct
{
    struct
    {
        char *value_p;
        bool enabled;
    } emergency_stop;
    struct
    {
        uint16_t milliseconds_delay;
        char *value_p;
        bool enabled;
    } deadman;
} tank_drive_settings;

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

void TankDriveSetDeadman(char *deadman, const uint16_t milliseconds_delay)
{
    if(!deadman)
        ESP_LOGE(TAG, "NULL deadman");

    tank_drive_settings.deadman.value_p = deadman;
    tank_drive_settings.deadman.milliseconds_delay = milliseconds_delay;
    tank_drive_settings.deadman.enabled = true;
}

void TankDriveSetEmergencyStop(char *emergency_stop)
{
    if(!emergency_stop)
        ESP_LOGE(TAG, "NULL emergency stop");

    tank_drive_settings.emergency_stop.value_p = emergency_stop;
    tank_drive_settings.emergency_stop.enabled = true;
}
