#include "task.h"

task_t KernelTask = {
    .pid = 0,
};

task_t* CurrentTask = &KernelTask;
