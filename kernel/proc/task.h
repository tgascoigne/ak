#pragma once

#include <unistd.h>

#include <proc/task.h>
#include <mem/types.h>

#define KERNEL_PID 0

typedef struct {
	pid_t pid;
	vaddr_t brk;
	arch_task_t;
} task_t;

extern task_t *CurrentTask;
extern task_t KernelTask;