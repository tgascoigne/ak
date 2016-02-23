#pragma once

#include <unistd.h>
#include <setjmp.h>
#include <signal.h>
#include <linux/limits.h>

#include <proc/task.h>
#include <mem/types.h>
#include <kernel/list.h>
#include <kernel/io/fdio.h>

#define KERNEL_PID 0

typedef enum {
	TaskInit,
	TaskReady,
	TaskSigWait,
	TaskQuit,
} tstate_t;

typedef struct {
	pid_t pid;
	pid_t ppid;
	int childcount;
	vaddr_t brk;
	tstate_t state;
	list_head_t list;
	fd_t fdnext;
	fd_t fdcap;
	fdescr_t **fdtbl;
	int exitcode;
	pid_t sid;
	pid_t pgid;
	union {
		arch_task_t;
		arch_task_t arch;
	};
	char cwd[PATH_MAX];
	vaddr_t mmapbrk;
	sigset_t sigset;
	struct sigaction sigact[NSIG];
	int sigpend;
	int sigwait;
	int sigchldpid;
	int sigchldexit;
} task_t;

extern task_t *CurrentTask;
extern task_t KernelTask;
extern task_t *UserTask;

pid_t task_next_pid(void);
void task_enter(task_t *task);
task_t *task_get(pid_t pid);
task_t *task_fork(void);
void task_insert(task_t *task);
void task_exit(task_t *task, int status);
void task_enable_preempt(void);
void task_mask_preempt(void);
void task_unmask_preempt(void);
task_t *task_clone(task_t *task);
__sighandler_t task_signal(task_t *task, int signum, __sighandler_t handler);
void task_signal_exec(task_t *task);
void task_signal_raise(task_t *task, int signum);
void task_sigwait(task_t *task);
