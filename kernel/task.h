#pragma once

#include <unistd.h>

#pragma message("transparent arch header access")
#include <arch/x86/task.h>
#include <arch/x86/types.h>

typedef struct {
	pid_t pid;
	vaddr_t brk;
	arch_task_t;
} task_t;

extern task_t* CurrentTask;
extern task_t KernelTask;
