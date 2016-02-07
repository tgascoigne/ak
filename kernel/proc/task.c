#include "task.h"

#include <stdlib.h>
#include <string.h>

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

task_t *task_clone(task_t *task) {
	task_t *clone = (task_t *)malloc(sizeof(task_t));
	clone->pid    = task_next_pid();
	clone->brk    = CurrentTask->brk;
	clone->fdcap  = CurrentTask->fdcap;
	clone->fdnext = CurrentTask->fdnext;
	clone->fdtbl = (fdescr_t **)malloc(clone->fdcap * sizeof(fdescr_t *));
	memcpy(clone->fdtbl, CurrentTask->fdtbl, clone->fdcap * sizeof(fdescr_t *));
	for (int i = 0; i < clone->fdnext; i++) {
		if (clone->fdtbl[i] != 0) {
			clone->fdtbl[i]->refs++;
		}
	}

	return clone;
}
