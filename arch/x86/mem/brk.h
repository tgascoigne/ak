#pragma once

#include <mem/types.h>
#include <kernel/task.h>

vaddr_t task_brk(task_t *task, vaddr_t brk);
