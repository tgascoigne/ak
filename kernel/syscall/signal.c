#include "signal.h"

#include <signal.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <kernel/syscall/syscall.h>
#include <kernel/proc/task.h>

pid_t sys_wait4(pid_t pid, int *status, int options, void *rusage) {
	if ((options & WNOHANG) != 0 || (options & WUNTRACED) != 0) {
		if (CurrentTask->childcount == 0) {
			return 0;
		}
	}
	CurrentTask->sigwait = SIGCHLD;
	task_sigwait(CurrentTask);
	*status = CurrentTask->sigchldexit;
	return CurrentTask->sigchldpid;
}

__sighandler_t sys_signal(int signum, __sighandler_t handler) {
	return task_signal(CurrentTask, signum, handler);
}

int sys_rt_sigsuspend(sigset_t *unewset, size_t sigsetsize) {
	task_sigwait(CurrentTask);
	return 0;
}

int sys_rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
	if (oldact) {
		*oldact = CurrentTask->sigact[signum];
	}
	CurrentTask->sigact[signum] = *act;
	return 0;
}

void signal_init(void) {
	syscall_register(SYS_WAIT4, (syscall_fn_t)sys_wait4, 4);
	syscall_register(SYS_SIGNAL, (syscall_fn_t)sys_signal, 2);
	syscall_register(SYS_RT_SIGACTION, (syscall_fn_t)sys_rt_sigaction, 3);
	syscall_register(SYS_RT_SIGSUSPEND, (syscall_fn_t)sys_rt_sigsuspend, 2);
	memset(CurrentTask->sigact, 0, NSIG * sizeof(struct sigaction));
}
