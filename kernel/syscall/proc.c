#include <stdbool.h>

#include <kernel/module.h>
#include <kernel/syscall/syscall.h>
#include <kernel/proc/task.h>

static task_t *last_task = (task_t *)NULL;

pid_t sys_fork(void) {
	task_t *clone = task_fork();
	if (clone) {
		last_task = clone;
		return clone->pid;
	}
	return 0;
}

int sys_sched_yield(void) {
	task_t *next_task = last_task;
	last_task = CurrentTask;
	task_enter(next_task);
	return 0;
}

static bool proc_init(void) {
	syscall_register(SYS_FORK, (syscall_fn_t)sys_fork, 0);
	syscall_register(SYS_SCHED_YIELD, (syscall_fn_t)sys_sched_yield, 0);
	return true;
}

module_init_prio(proc_init, MODINIT_EARLY);
