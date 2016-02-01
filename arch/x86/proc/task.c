#include "task.h"

#include <stdlib.h>

#include <arch/x86/mem/mmu.h>
#include <kernel/proc/task.h>

void task_enter(task_t *task) {
	task_t *next_task;
	next_task = (task_t *)setjmp(CurrentTask->ctx);
	if (next_task == 0) {
		longjmp(task->ctx, (int)task);
	}
	CurrentTask = next_task;
	pg_switch_dir(next_task->pgd);
}

task_t *task_fork(void) {
	task_t *clone  = (task_t *)malloc(sizeof(task_t));
	clone->pid     = task_next_pid();
	clone->brk     = CurrentTask->brk;
	clone->console = CurrentTask->console;
	clone->pgd     = pg_clone_dir(CurrentTask->pgd);

	if (setjmp(clone->ctx) != 0) {
		/* Reached when the scheduler longjmps to the new process */
		/* set clone = 0 to signal this to caller */
		pg_switch_dir(clone->pgd);
		CurrentTask = clone;
		clone       = 0;
	}
	return clone;
}
