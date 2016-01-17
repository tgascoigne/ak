#pragma once

#ifndef ASM_FILE
#include <arch/x86/mem/types.h>
#endif

/* A PDE maps a 4m block (2^22 = 4m) */
#define ADDR_PDE(addr) (addr >> 22)
/* A PTE maps a 4k block (2^12 = 4k) */
#define ADDR_PTE(addr) ((addr >> 12) % ADDR_PDE(addr))

#define MEMMAX (0xFFFFFFFF)

#define KZERO (0xC0000000)
#define KTEXT (0x00100000 + KZERO)
/* The last 4m is used as a temporary mapping area */
#define KTMPMEM (MEMMAX - 0x400000)

#define KBSSTOPHYS(addr) ((paddr_t)addr - KZERO)
