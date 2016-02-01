#pragma once

#include <setjmp.h>

#include <arch/x86/mem/mmu.h>

struct task;

typedef struct {
	pgaddr_t pgd;
	jmp_buf ctx;
} arch_task_t;
