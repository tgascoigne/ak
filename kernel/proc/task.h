#pragma once

#include <unistd.h>
#include <setjmp.h>
#include <linux/limits.h>

#include <proc/task.h>
#include <mem/types.h>
#include <kernel/list.h>
#include <kernel/io/fdio.h>

#define KERNEL_PID 0

typedef enum {
	TaskInit,
	TaskReady,
	TaskQuit,
} tstate_t;

typedef struct {
	pid_t pid;
	pid_t ppid;
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
