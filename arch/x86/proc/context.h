#pragma once

#include <stdint.h>

#include <arch/x86/mem/types.h>
#include <kernel/proc/task.h>

int task_store_context(task_t *task);
int task_load_context(task_t *task);
int task_fork_context(task_t *task);
