#ifndef DRIVE_HARDWARE_DEFINITIONS_H
#define DRIVE_HARDWARE_DEFINITIONS_H

#include "driver/gpio.h"

// Motor drivers

// Left motor

#define L_IN_A_PIN  GPIO_NUM_12
#define L_IN_B_PIN  GPIO_NUM_14
#define L_MODE_PIN  GPIO_NUM_27
#define L_SLEEP_PIN GPIO_NUM_26

// Right motor

#define R_MODE_PIN  GPIO_NUM_21
#define R_SLEEP_PIN GPIO_NUM_19
#define R_IN_A_PIN  GPIO_NUM_18
#define R_IN_B_PIN  GPIO_NUM_5

#endif  // DRIVE_HARDWARE_DEFINITIONS_H
