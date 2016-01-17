#pragma once

#include <arch/x86/mem/mmu.h>

typedef struct { pgentry_t *pgd; } arch_task_t;
