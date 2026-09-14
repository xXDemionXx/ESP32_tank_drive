# Tank Drive

![Platform](https://img.shields.io/badge/platform-ESP32-orange)

An ESP-IDF component for controlling a two-motor differential/tank drive.

The tank drive periodically gets a `tank_drive_move_command_t`, calculates the required left and right motor speeds, and sends them to the motors.

The component is **controller-independent**. It does not require a specific controller or input device. Any component can provide the movement command as long as it implements a compatible `tank_drive_move_command_getter`.

## Features

* Two-motor differential/tank drive
* Software deadman switch
* Emergency stop
* Kconfig configuration

## Dependencies

The tank drive component depends on:

* `motor_control`
* `tank_drive_move_command`

## How it works

The tank drive receives movement commands in the form of:

```c
typedef struct
{
    float T;
    float R;
} tank_drive_move_command_t;
```

`T` represents translation and `R` represents rotation.

Both values should normally be in the range `-1.0` to `1.0`.

* `T = 1.0` → maximum forward movement
* `T = 0.0` → no translation
* `T = -1.0` → maximum backward movement
* `R = 1.0` → maximum rotation in the configured positive direction
* `R = 0.0` → no rotation
* `R = -1.0` → maximum rotation in the configured negative direction

The tank drive converts `T` and `R` into individual left and right motor speeds.

---

# Movement command getter

The tank drive does not directly read a controller.

Instead, it receives a function pointer:

```c
typedef tank_drive_move_command_t (*tank_drive_move_command_getter)(void);
```

This function is periodically called by the tank-drive task to obtain the current movement command.

For example:

```c
tank_drive_move_command_t get_move_command(void)
{
    tank_drive_move_command_t command = {
        .T = 0.5f,
        .R = 0.0f
    };

    return command;
}
```

The function can then be passed to:

```c
TankDriveInit(get_move_command);
```

This makes the tank drive independent of where the movement commands come from.

They could come from:

* A game controller
* A joystick
* A remote-control system
* A network connection
* An autonomous navigation algorithm
* A custom control component

---

# Using a controller

If you are using a physical controller, I recommend using the **Controller Tank Drive Interface** component as middleware between your controller component and the tank drive.

The architecture is:

```text
Controller
    │
    ▼
Controller component
    │
    ▼
Controller Tank Drive Interface
    │
    │ tank_drive_move_command_getter
    ▼
Tank Drive
    │
    ▼
Motor Control
    │
    ▼
Motors
```

The `controller_tank_drive_interface` component is intended to act as **middleware**. It is not tied to a single controller implementation and can work with different controller components.

For example, it can be used together with my **DualShock 4 ESP32** component.

The Controller Tank Drive Interface converts controller-specific input into the generic `tank_drive_move_command_t` expected by the tank drive.

You can find the project here:

* [Controller Tank Drive Interface](https://github.com/xXDemionXx/controller_tank_drive_interface)
* [DualShock 4 ESP32](https://github.com/xXDemionXx/DualShock4_ESP32)

If you are not using a controller, or if the existing middleware does not suit your application, you can simply implement your own `tank_drive_move_command_getter`.

---

# Example using Controller Tank Drive Interface

The following example uses:

* A DualShock 4 controller
* My DualShock 4 ESP32 component
* `controller_tank_drive_interface`
* The `joy_angle_and_gas` control scheme
* The tank drive component

The controller-specific data is provided by the DualShock 4 component. The Controller Tank Drive Interface then converts that data into a tank-drive movement command.

```c
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ds4.h"
#include "controller_tank_drive_interface.h"
#include "tank_drive.h"

ds4_data_t data;

tank_drive_move_command_getter TR_getter;

char deadman_L1(void)
{
    if (data.buttons.L1 == 1)
        return 1;
    else
        return 0;
}

char emergency_stop_triangle(void)
{
    if (data.buttons.triangle == 1)
        return 0;
    else
        return 1;
}

void app_main(void)
{
    // Initialize the DualShock 4 component
    ds4Init();
    ds4SetPollingStruct(&data);

    // Configure safety features
    TankDriveSetDeadman(deadman_L1, 300, 300);
    TankDriveSetEmergencyStop(emergency_stop_triangle);

    // Wait for the controller to connect
    while (ds4GetConnectionStatus() != DS4_READY)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Create the tank-drive command getter.
    //
    // The Controller Tank Drive Interface handles the conversion
    // from DualShock 4 joystick/gas values to T/R values.
    TR_getter = ControllerTankDriveInterfaceSetup(
        setup_joy_angle_and_gas,
        joy_angle_and_gas_dualshock4_default_config(
            &data.ljoy.x,
            &data.ljoy.y,
            &data.R2));

    // Start the tank drive
    TankDriveInit(TR_getter);
}
```

The important part is that the tank drive itself does not know that a DualShock 4 is being used:

```c
TankDriveInit(TR_getter);
```

The data flow is:

```text
DualShock 4
     │
     │ controller data
     ▼
DualShock 4 component
     │
     ▼
Controller Tank Drive Interface
     │
     │ T / R
     ▼
Tank Drive
```

This separation allows the same tank-drive component to be reused with completely different controllers.

---

# Custom movement command source

You do **not** have to use `controller_tank_drive_interface`.

You can create your own getter:

```c
tank_drive_move_command_t my_getter(void)
{
    tank_drive_move_command_t command;

    command.T = my_translation_value;
    command.R = my_rotation_value;

    return command;
}
```

Then:

```c
void app_main(void)
{
    TankDriveInit(my_getter);
}
```

For example, an autonomous robot could completely skip the controller layer:

```text
Autonomous navigation
        │
        ▼
  my_getter()
        │
        ▼
    Tank Drive
        │
        ▼
   Motor Control
```

This is one of the main design goals of the component: **Tank Drive only cares about the final movement command, not how that command was generated.**

---

# Deadman switch

A software deadman switch can be configured using:

```c
void TankDriveSetDeadman(
    char (*deadman_getter)(void),
    uint16_t ms_hold_delay,
    uint16_t ms_release_delay);
```

The getter should return:

* non-zero → deadman is pressed
* `0` → deadman is released

Example:

```c
char deadman_L1(void)
{
    return data.buttons.L1;
}
```

Configure it with:

```c
TankDriveSetDeadman(deadman_L1, 300, 300);
```

The tank drive will stop when the deadman has remained released for the configured release delay.

After stopping, movement will resume only after the deadman has remained pressed for the configured hold delay.

The exact timing can be slightly longer than the requested delay because the deadman is checked periodically by the tank-drive task.

---

# Emergency stop

The emergency stop is a software input that permanently stops the tank drive until the ESP32 is restarted.

```c
void TankDriveSetEmergencyStop(
    char (*emergency_stop_getter)(void));
```

The getter should return:

* `0` → normal operation
* non-zero → emergency stop

Example:

```c
char emergency_stop(void)
{
    return emergency_stop_button;
}
```

Configure it with:

```c
TankDriveSetEmergencyStop(emergency_stop);
```

Once the emergency-stop getter returns a non-zero value, the tank drive stops and cannot resume during the current boot.

---

# Kconfig

Configure the component with:

```text
idf.py menuconfig
```

The settings are located under:

```text
Tank drive
```

## Task

### `TANK_DRIVE_TASK_PRIORITY`

Priority of the tank-drive task.

Default:

```text
12
```

### `TANK_DRIVE_TASK_LOOP_PERIOD_MS`

Period between motor speed calculations in milliseconds.

Default:

```text
10 ms
```

A shorter period gives more frequent motor updates but increases the task's execution frequency.

---

# Movement settings

## `TANK_DRIVE_INVERT_FORWARD_BACKWARD`

By default:

```text
T > 0 → Forward
T < 0 → Backward
```

Enable this option to reverse the forward/backward direction.

## `TANK_DRIVE_INVERT_ROTATION`

Controls the direction associated with positive and negative `R`.

---

# Left motor settings

The left motor has independent configuration for:

* Direction inversion
* Minimum PWM percentage
* Maximum PWM percentage
* `IN_A` GPIO
* `IN_B` GPIO
* `MODE` GPIO
* `SLEEP` GPIO

### Minimum PWM

Some motors do not start rotating at very low PWM values.

For example:

```text
Minimum PWM = 25%
Maximum PWM = 100%
```

causes the usable motor-control range to be mapped between 25% and 100% PWM.

### GPIO configuration

The following GPIOs can be configured through Kconfig:

```text
TANK_DRIVE_LEFT_MOTOR_IN_A_PIN
TANK_DRIVE_LEFT_MOTOR_IN_B_PIN
TANK_DRIVE_LEFT_MOTOR_MODE_PIN
TANK_DRIVE_LEFT_MOTOR_SLEEP_PIN
```

---

# Right motor settings

The right motor has the same configuration options as the left motor:

* Direction inversion
* Minimum PWM percentage
* Maximum PWM percentage
* `IN_A` GPIO
* `IN_B` GPIO
* `MODE` GPIO
* `SLEEP` GPIO

The GPIOs can be configured through:

```text
TANK_DRIVE_RIGHT_MOTOR_IN_A_PIN
TANK_DRIVE_RIGHT_MOTOR_IN_B_PIN
TANK_DRIVE_RIGHT_MOTOR_MODE_PIN
TANK_DRIVE_RIGHT_MOTOR_SLEEP_PIN
```

---

# API

## `TankDriveInit`

```c
void TankDriveInit(tank_drive_move_command_getter getter);
```

Initializes and starts the tank-drive task.

`getter` is the function used to obtain the current movement command.

It is safe to call the other tank-drive configuration functions before this function. The tank drive remains inactive until `TankDriveInit()` is called.

---

## `TankDriveSetDeadman`

```c
void TankDriveSetDeadman(
    char (*deadman_getter)(void),
    uint16_t ms_hold_delay,
    uint16_t ms_release_delay);
```

Configures the software deadman switch.

* `deadman_getter` — function used to get the deadman's state
* `ms_hold_delay` — minimum time the deadman must remain pressed before movement can resume
* `ms_release_delay` — minimum time the deadman must remain released before movement stops

The delays are minimum times and depend on the tank-drive task period.

---

## `TankDriveSetEmergencyStop`

```c
void TankDriveSetEmergencyStop(
    char (*emergency_stop_getter)(void));
```

Configures the emergency-stop input.

If the getter returns a non-zero value, the tank drive stops permanently until the ESP32 is restarted.

---

# Design

The component is intentionally split into several layers:

```text
┌─────────────────────────────┐
│ Controller / Autonomous code│
└──────────────┬──────────────┘
               │
               │ controller-specific data
               ▼
┌─────────────────────────────┐
│ Controller Tank Drive       │
│ Interface (optional)        │
└──────────────┬──────────────┘
               │
               │ tank_drive_move_command_t
               ▼
┌─────────────────────────────┐
│ Tank Drive                  │
└──────────────┬──────────────┘
               │
               │ motor speeds
               ▼
┌─────────────────────────────┐
│ Motor Control               │
└──────────────┬──────────────┘
               │
               ▼
          Left / Right
             Motors
```

The Controller Tank Drive Interface is **optional middleware**. It is recommended when using a controller because it separates controller-specific input handling from the tank-drive implementation.

However, the tank drive itself has no dependency on a particular controller and can be connected directly to any custom input or control algorithm that provides a `tank_drive_move_command_getter`.
