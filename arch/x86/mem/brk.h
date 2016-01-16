#pragma once

#include <arch/x86/types.h>
#include <kernel/task.h>

vaddr_t task_brk(task_t *task, vaddr_t brk);
