#include "sched.h"

#include <stdbool.h>

#include <kernel/proc/task.h>

void kern_sched_yield(void) {
	task_t *next_task = CurrentTask;
	while (next_task->state != TaskReady) {
		next_task = list_elem(next_task->list.next, task_t, list);

		if (next_task->state == TaskSigWait && next_task->sigpend != 0 &&
		    (next_task->sigwait == 0 || next_task->sigpend == next_task->sigwait)) {
			break;
		}
	}
	task_enter(next_task);
}

void sched_init(void) {
	KernelTask.list.next = &KernelTask.list;
	KernelTask.list.prev = &KernelTask.list;
	task_enable_preempt();
}
