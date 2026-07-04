#ifndef KINEMATICS_TASK_H
#define KINEMATICS_TASK_H

// Public definitions
#define KINEMATICS_TASK_PRIORITY    12
#define KINEMATICS_TASK_NAME        "kinematics_task"
#define KINEMATICS_TASK_SIZE        10*1024  // In bytes

// Public function prototypes

void kinematics_task(void *p_param);

#endif  // KINEMATICS_TASK_H
