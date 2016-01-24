#include "task.h"

#include <mem/mmu.h>

extern void *_kern_end;

task_t KernelTask = {
    .pid = KERNEL_PID, .pgd = KernelPageDir, .brk = (vaddr_t)&_kern_end,
};

task_t *CurrentTask = &KernelTask;
