#include "proc.h"

#include <sched.h>
#include <stdbool.h>
#include <errno.h>

#include <arch/i386/intr/intr.h>
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

pid_t sys_clone(unsigned long flags, void *child_stack, void *ptid, void *ctid, void *regs) {
#pragma message("clone() isn't implemented properly")
	return sys_fork();
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
	intr_enable();
	while (1) {
	}
	return 0;
}

int sys_exit_group(int status) {
    return sys_exit(status);
}

int sys_sched_yield(void) {
	kern_sched_yield();
	return 0;
}

pid_t sys_getppid(void) {
	return CurrentTask->ppid;
}

pid_t sys_getpid(void) {
	return CurrentTask->pid;
}

pid_t sys_getpgid(pid_t pid) {
	if (pid == 0) {
		pid = CurrentTask->pid;
	}
	task_t *task = task_get(pid);
	if (task == NULL) {
		errno = ESRCH;
		return -1;
	}

	return task->pid;
}

pid_t sys_getpgrp(void) {
	return sys_getpgid(0);
}

int sys_setpgid(pid_t pid, pid_t pgid) {
	if (pid == 0) {
		pid = CurrentTask->pid;
	}
	task_t *task = task_get(pid);
	if (task == NULL) {
		errno = ESRCH;
		return -1;
	}

	if (pgid == 0) {
		pgid = task->pid;
	}

	task->pgid = pgid;
	return 0;
}

void proc_init(void) {
	syscall_register(SYS_EXIT, (syscall_fn_t)sys_exit, 1);
	syscall_register(SYS_EXECVE, (syscall_fn_t)sys_execve, 3);
	syscall_register(SYS_GETPPID, (syscall_fn_t)sys_getppid, 0);
	syscall_register(SYS_GETPID, (syscall_fn_t)sys_getpid, 0);
	syscall_register(SYS_FORK, (syscall_fn_t)sys_fork, 0);
	syscall_register(SYS_CLONE, (syscall_fn_t)sys_clone, 0);
	syscall_register(SYS_SCHED_YIELD, (syscall_fn_t)sys_sched_yield, 0);
	syscall_register(SYS_GETPGID, (syscall_fn_t)sys_getpgid, 1);
	syscall_register(SYS_GETPGRP, (syscall_fn_t)sys_getpgrp, 0);
	syscall_register(SYS_SETPGID, (syscall_fn_t)sys_setpgid, 2);
	syscall_register(SYS_EXIT_GROUP, (syscall_fn_t)sys_exit_group, 1);
}
