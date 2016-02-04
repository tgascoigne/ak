#include "sched.h"

#include <stdbool.h>

#include <kernel/proc/task.h>

void kern_sched_yield(void) {
	task_t *next_task = list_elem(CurrentTask->list.next, task_t, list);
	task_enter(next_task);
}

void sched_init(void) {
	KernelTask.list.next = &KernelTask.list;
	KernelTask.list.prev = &KernelTask.list;
	task_enable_preempt();
}
