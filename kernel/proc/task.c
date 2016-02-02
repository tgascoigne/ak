#include "task.h"

#include <mem/mmu.h>

extern void *_kern_end;

task_t KernelTask = {
    .pid	  = KERNEL_PID,
    .brk	  = (vaddr_t)&_kern_end,
    .state = TaskReady,
    .list =
	{
	    .next = NULL, .prev = NULL,
	},
};

task_t *CurrentTask = &KernelTask;

static pid_t next_pid = KERNEL_PID + 1;

pid_t task_next_pid(void) {
	return next_pid++;
}

void task_insert(task_t *task) {
	list_insert_after(&CurrentTask->list, &task->list);
}
