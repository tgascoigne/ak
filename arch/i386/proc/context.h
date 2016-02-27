#pragma once

#include <stdint.h>

#include <arch/i386/mem/types.h>
#include <kernel/proc/task.h>

int task_store_context(arch_task_t *task);
int task_load_context(arch_task_t *task);
int task_fork_context(arch_task_t *task);
