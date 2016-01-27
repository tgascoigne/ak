#pragma once

#if !defined(ASM_FILE) && !defined(LD_SCRIPT)
#include <arch/x86/mem/types.h>
#endif

#define MEMMAX (0xFFFFFFFF)

#define KZERO (0xC0000000)
#define KTEXT (0x00100000 + KZERO)
/* The last 4m is used as a temporary mapping area */
#define KTMPMEM (MEMMAX - (0x400000 - 1))

#define KSTACK (KZERO)

#define VGA_TEXT_BASE (0xB8000 + KZERO)

#define KBSSTOPHYS(addr) ((paddr_t)addr - KZERO)
