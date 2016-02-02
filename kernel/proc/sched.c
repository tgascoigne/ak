#include "sched.h"

#include <stdbool.h>

#include <kernel/module.h>
#include <kernel/proc/task.h>

void kern_sched_yield(void) {
	task_t *next_task = list_elem(CurrentTask->list.next, task_t, list);
	task_enter(next_task);
}

static bool sched_init(void) {
	KernelTask.list.next = &KernelTask.list;
	KernelTask.list.prev = &KernelTask.list;
	task_enable_preempt();
	return true;
}

module_init_prio(sched_init, MODINIT_LATE);
