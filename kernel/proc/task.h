#pragma once

#include <unistd.h>
#include <setjmp.h>

#include <proc/task.h>
#include <mem/types.h>
#include <kernel/io/fdio.h>

#define KERNEL_PID 0

typedef struct {
	pid_t pid;
	vaddr_t brk;
	fdescr_t *console;
	arch_task_t;
} task_t;

extern task_t *CurrentTask;
extern task_t KernelTask;

pid_t task_next_pid(void);
void task_enter(task_t *task);
task_t *task_fork(void);
