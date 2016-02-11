#include "proc.h"

#include <stdbool.h>

#include <arch/x86/intr/intr.h>
#include <kernel/syscall/syscall.h>
#include <kernel/proc/task.h>
#include <kernel/proc/sched.h>
#include <kernel/proc/elf/exec.h>

pid_t sys_fork(void) {
	task_t *clone = task_fork();
	if (clone) {
		return clone->pid;
	}
	return 0;
}

int sys_execve(const char *filename, char *const argv[], char *const envp[]) {
	elf_t bin;
	if (!elf_read(filename, &bin)) {
		return -1;
	}

	elf_load(&bin);
	elf_start(&bin, argv, envp);

	/* never reached */
	return -1;
}

int sys_exit(int status) {
	task_exit(CurrentTask, status);
	/* this system call should never return.
	   at this point, the task is removed from the queue, so we're
	   just waiting out it's last cycle on the scheduler */
	while (1) {
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

void proc_init(void) {
	syscall_register(SYS_EXIT, (syscall_fn_t)sys_exit, 1);
	syscall_register(SYS_EXECVE, (syscall_fn_t)sys_execve, 3);
	syscall_register(SYS_GETPID, (syscall_fn_t)sys_getpid, 0);
	syscall_register(SYS_FORK, (syscall_fn_t)sys_fork, 0);
	syscall_register(SYS_SCHED_YIELD, (syscall_fn_t)sys_sched_yield, 0);
}
