#include "mmu.h"

#include <arch/x86/mem/map.h>

#define KERNEL_FRAME PAGE_ENTRY(0, PAGE_PRESENT | PAGE_RW | PAGE_EXTENDED | PAGE_LINK)

// boot_pgd is the initial page directory used during startup
pgentry_t boot_pgdir[1024] PAGE_ALIGN = {
	[ADDR_PDE(0)] = KERNEL_FRAME, [ADDR_PDE(KZERO)] = KERNEL_FRAME,
};
