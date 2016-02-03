#include <stdbool.h>

#include <arch/x86/intr/intr.h>
#include <kernel/module.h>
#include <kernel/syscall/syscall.h>
#include <kernel/proc/task.h>
#include <kernel/proc/sched.h>

pid_t sys_fork(void) {
	task_t *clone = task_fork();
	if (clone) {
		return clone->pid;
	}
	return 0;
}

int sys_sched_yield(void) {
	kern_sched_yield();
	return 0;
}

pid_t sys_getpid(void) {
	return CurrentTask->pid;
}

static bool proc_init(void) {
	syscall_register(SYS_GETPID, (syscall_fn_t)sys_getpid, 0);
	syscall_register(SYS_FORK, (syscall_fn_t)sys_fork, 0);
	syscall_register(SYS_SCHED_YIELD, (syscall_fn_t)sys_sched_yield, 0);
	return true;
}

module_init_prio(proc_init, MODINIT_EARLY);
