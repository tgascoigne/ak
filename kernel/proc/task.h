#pragma once

#include <unistd.h>
#include <setjmp.h>

#include <proc/task.h>
#include <mem/types.h>
#include <kernel/list.h>
#include <kernel/io/fdio.h>

#define KERNEL_PID 0

typedef enum {
	TaskInit,
	TaskReady,
} tstate_t;

typedef struct {
	pid_t pid;
	vaddr_t brk;
	tstate_t state;
	list_head_t list;
	union {
		arch_task_t;
		arch_task_t arch;
	};
} task_t;

extern task_t *CurrentTask;
extern task_t KernelTask;

pid_t task_next_pid(void);
void task_enter(task_t *task);
task_t *task_fork(void);
void task_insert(task_t *task);
void task_enable_preempt(void);
void task_mask_preempt(void);
void task_unmask_preempt(void);
