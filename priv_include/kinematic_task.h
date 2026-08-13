#ifndef KINEMATIC_TASK_H
#define KINEMATIC_TASK_H
#include "sdkconfig.h"

// Public definitions
#define KINEMATICS_TASK_PRIORITY CONFIG_TANK_DRIVE_TASK_PRIORITY
#define KINEMATICS_TASK_NAME "kinematics_task"
#define KINEMATICS_TASK_SIZE 10 * 1024 // In bytes

// Public function prototypes

void kinematic_task(void *p_param);

#endif // KINEMATIC_TASK_H
