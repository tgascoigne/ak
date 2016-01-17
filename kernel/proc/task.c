#include "task.h"

task_t KernelTask = {
    .pid	  = KERNEL_PID,
};

task_t *CurrentTask = &KernelTask;
