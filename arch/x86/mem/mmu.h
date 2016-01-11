#pragma once

#ifndef ASM_FILE
#include <stdint.h>
#endif

/* Page entry fields */
#define PAGE_PRESENT (1 << 0)
#define PAGE_RW (1 << 1)
#define PAGE_USER (1 << 2)
#define PAGE_WRITETHROUGH (1 << 3)
#define PAGE_NOCACHE (1 << 4)
#define PAGE_ACCESSED (1 << 5)
#define PAGE_DIRTY (1 << 6)
#define PAGE_EXTENDED (1 << 7)
#define PAGE_GLOBAL (1 << 7)
/* ak specific fields */
#define PAGE_RESERVED (1 << 8) /* A 'reserved' page is lazy allocated */
#define PAGE_LINK (1 << 9)     /* A linked page is always linked on pd/pt copy */
/* (1 << 10) is unused */

#ifndef ASM_FILE
typedef uint32_t pgentry_t;
#endif

/* Control register flags */
#define CR4_PSE 0x10
#define CR0_PGENABLE 0x80000000

#define PAGE_SIZE 0x1000

#ifndef ASM_FILE
#define PAGE_ALIGN __attribute__((aligned(PAGE_SIZE)))
#endif

/* sugar to help define pgentry_t's */
#define PAGE_ENTRY(addr, flags) (pgentry_t)(addr | (flags))
