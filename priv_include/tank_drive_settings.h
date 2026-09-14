#ifndef TANK_DRIVE_SETTINGS_H
#define TANK_DRIVE_SETTINGS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    struct
    {
        char (*getter)(void);
        bool enabled;
    } emergency_stop;
    struct
    {
        uint16_t ms_hold_delay;
        uint16_t ms_release_delay;
        char (*getter)(void);
        bool enabled;
    } deadman;
} tank_drive_settings_t;

extern tank_drive_settings_t _tank_drive_settings;

#endif // TANK_DRIVE_SETTINGS_H
