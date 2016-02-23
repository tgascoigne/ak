#include "sched.h"

#include <stdbool.h>

#include <intr/intr.h>
#include <kernel/proc/task.h>

void kern_sched_yield(void) {
	intr_disable();
	task_t *next_task = CurrentTask;
	do {
		next_task = list_elem(next_task->list.next, task_t, list);

		if (next_task->state == TaskQuit) {
			list_head_t *real_next = next_task->list.next;
			list_remove(&next_task->list);
			next_task->list.next = real_next;
		}

		if (next_task->state == TaskSigWait && next_task->sigpend != 0) {
			/* task is waiting on a signal and one is pending */
			if (next_task->sigpend == next_task->sigwait || next_task->sigwait == 0) {
				next_task->state = TaskReady;
				break;
			}
		}
	} while (next_task->state != TaskReady);
	task_enter(next_task);
	intr_enable();
}

void sched_init(void) {
	KernelTask.list.next = &KernelTask.list;
	KernelTask.list.prev = &KernelTask.list;
	task_enable_preempt();
}
