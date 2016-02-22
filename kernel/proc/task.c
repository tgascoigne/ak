#include "task.h"

#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mem/mmu.h>

extern void *_kern_end;

task_t KernelTask = {
    .pid          = KERNEL_PID,
    .ppid         = KERNEL_PID,
    .brk          = (vaddr_t)&_kern_end,
    .state = TaskReady,
    .list =
        {
            .next = NULL, .prev = NULL,
        },
    .cwd     = "/",
    .sid     = 1,
    .pgid    = 1,
    .mmapbrk = 0,
};

task_t *CurrentTask = &KernelTask;
task_t *UserTask    = NULL;

static pid_t next_pid = KERNEL_PID + 1;

pid_t task_next_pid(void) {
	return next_pid++;
}

void task_insert(task_t *task) {
	list_insert_after(&CurrentTask->list, &task->list);
}

task_t *task_get(pid_t pid) {
	task_t *next_task = CurrentTask;
	do {
		next_task = list_elem(next_task->list.next, task_t, list);
		if (next_task->pid == pid) {
			return next_task;
		}
	} while (next_task->pid != CurrentTask->pid);
	return (task_t *)NULL;
}

void task_exit(task_t *task, int status) {
	task->state    = TaskQuit;
	task->exitcode = status;
	task_signal_raise(task, SIGTERM);

	task_t *parent = task_get(task->ppid);
	if (!parent) {
		printf("failed to signal SIGCHLD to parent: no such pid %i\n", task->ppid);
	} else {
		task_signal_raise(parent, SIGCHLD);
	}

	list_remove(&task->list);
	printf("task %i exited with code %i\n", task->pid, status);
}

task_t *task_clone(task_t *task) {
	task_t *clone  = (task_t *)malloc(sizeof(task_t));
	clone->pid     = task_next_pid();
	clone->ppid    = CurrentTask->pid;
	clone->brk     = CurrentTask->brk;
	clone->mmapbrk = CurrentTask->mmapbrk;
	clone->sid     = CurrentTask->sid;
	clone->pgid    = CurrentTask->pgid;
	clone->sigset  = CurrentTask->sigset;
	clone->state = TaskReady;
	memcpy(clone->sighandlers, CurrentTask->sighandlers, NSIG * sizeof(__sighandler_t));
	strncpy(clone->cwd, CurrentTask->cwd, PATH_MAX);

	clone->fdcap  = CurrentTask->fdcap;
	clone->fdnext = CurrentTask->fdnext;
	clone->fdtbl = (fdescr_t **)malloc(clone->fdcap * sizeof(fdescr_t *));
	memcpy(clone->fdtbl, CurrentTask->fdtbl, clone->fdcap * sizeof(fdescr_t *));
	for (int i = 0; i < clone->fdnext; i++) {
		if (clone->fdtbl[i] != 0) {
			clone->fdtbl[i]->refs++;
		}
	}

	return clone;
}

void task_signal_raise(task_t *task, int signum) {
	if (task->state == TaskSigWait) {
		/* task was waiting on a signal; wake it up */
		task->state = TaskReady;
	}
	task->sigpend = signum;
}

void task_signal_exec(task_t *task) {
	if (task->sigpend) {
		int signum = task->sigpend;

		if (task->sigwait == task->sigpend) {
			task->sigwait = 0;
		}

		task->sigpend = 0;

		__sighandler_t hndl = task->sighandlers[signum];
		if (hndl && hndl != SIG_IGN) {
			hndl(signum);
		}
	}
}

__sighandler_t task_signal(task_t *task, int signum, __sighandler_t handler) {
	__sighandler_t old        = task->sighandlers[signum];
	task->sighandlers[signum] = handler;
	return old;
}

void task_sigwait(task_t *task) {
	task->state = TaskSigWait;
	sched_yield();
}
