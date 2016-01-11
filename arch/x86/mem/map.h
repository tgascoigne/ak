#pragma once

/* A PDE maps a 4m block (2^22 = 4m) */
#define ADDR_PDE(addr) (addr >> 22)
/* A PTE maps a 4k block (2^12 = 4k) */
#define ADDR_PTE(addr) ((addr >> 12) % ADDR_PDE(addr))

#define KZERO (0xC0000000)
#define KTEXT (0x00100000 + KZERO)
#define MEMMAX (0xFFFFFFFF)
