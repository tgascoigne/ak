#include "signal.h"

#include <signal.h>
#include <stdbool.h>
#include <errno.h>

#include <kernel/syscall/syscall.h>
#include <kernel/proc/task.h>

pid_t sys_wait4(pid_t pid, int *status, int options, void *rusage) {
	CurrentTask->sigwait = SIGCHLD;
	task_sigwait(CurrentTask);
	*status = CurrentTask->exitcode;
	return 0;
}

__sighandler_t sys_signal(int signum, __sighandler_t handler) {
	return task_signal(CurrentTask, signum, handler);
}

int sys_rt_sigsuspend(sigset_t *unewset, size_t sigsetsize) {
	task_sigwait(CurrentTask);
	return 0;
}

void signal_init(void) {
	syscall_register(SYS_WAIT4, (syscall_fn_t)sys_wait4, 4);
	syscall_register(SYS_SIGNAL, (syscall_fn_t)sys_signal, 2);
	syscall_register(SYS_RT_SIGSUSPEND, (syscall_fn_t)sys_rt_sigsuspend, 2);
	memset(CurrentTask->sighandlers, 0, NSIG * sizeof(__sighandler_t));
}
