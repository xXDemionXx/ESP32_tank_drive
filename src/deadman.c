#include "deadman.h"
#include "tank_drive_settings.h"
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

bool get_deadman_state(void)
{
    static bool deadman = false; // Start in the OFF state
    static TickType_t xDeadmanLastHeldTime = 0;
    static TickType_t xDeadmanLastReleasedTime = 0;

    if (_tank_drive_settings.deadman.getter() == 0) // Deadman not pressed
    {
        xDeadmanLastReleasedTime = xTaskGetTickCount();
        if (xTaskGetTickCount() - xDeadmanLastHeldTime > pdMS_TO_TICKS(_tank_drive_settings.deadman.ms_release_delay))
        {
            // If deadman hasn't been held for enough time
            if (deadman == true)
            {
                // If it the deadman was ON. turn it OFF
                deadman = false;
            }
            return deadman;
        }
        else
        {
            // Not enough time passed after release
            return deadman;
        }
    }
    else // Deadman pressed
    {
        xDeadmanLastHeldTime = xTaskGetTickCount();
        if (xTaskGetTickCount() - xDeadmanLastReleasedTime > pdMS_TO_TICKS(_tank_drive_settings.deadman.ms_hold_delay))
        {
            // If deadman has been held for enough time
            if (deadman == false)
            {
                // If it the deadman was OFF. turn it ON
                deadman = true;
            }
            return deadman;
        }
        else
        {
            // Not enough time passed after pressing
            return deadman;
        }
    }
}
