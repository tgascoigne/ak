#pragma once

#ifndef ASM_FILE
#include <stdint.h>
#include <arch/x86/mem/types.h>
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
typedef paddr_t pgaddr_t;
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

#define PGADDR(addr) (pgaddr_t)(addr - (addr % PAGE_SIZE))

/* A PDE maps a 4m block (2^22 = 4m) */
#define ADDR_PDE(addr) (addr >> 22)
/* A PTE maps a 4k block (2^12 = 4k) */
#define ADDR_PTE(addr) ((addr >> 12) % 1024)
/* Extracts the address from a PDE */
#define PDE_ADDR(pde) (pde & 0xFFC00000);
/* Extracts the address from a PTE */
#define PTE_ADDR(pte) (pte & 0xFFFFF000);

#ifndef ASM_FILE
void pg_init(void);
void pg_map(pgaddr_t paddr, vaddr_t vaddr);
pgentry_t *pg_tmp_map(paddr_t addr);
void pg_tmp_unmap(pgentry_t *mapping);
void pg_flush_tlb(void);
void pg_invlpg(vaddr_t addr);
#endif
