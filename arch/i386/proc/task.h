#pragma once

#include <arch/i386/mem/mmu.h>

typedef struct {
	uint32_t esp;
	uint32_t ebp;
	pgaddr_t pgd;
} arch_task_t;
