#pragma once

#ifndef ASM_FILE
#include <arch/x86/mem/types.h>

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
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
#define PAGE_GLOBAL (1 << 8)
/* ak specific fields */
#define PAGE_RESERVED (1 << 8)   /* A 'reserved' page is lazy allocated */
#define PAGE_LINK (1 << 9)       /* A linked page is always linked on pd/pt copy */
#define PAGE_STACK_END (1 << 10) /* Stack boundary. Used to indicate a stack overflow and allocate more stack space */

#ifndef ASM_FILE
typedef uint32_t pgentry_t;
typedef paddr_t pgaddr_t;
#endif

/* Control register flags */
#define CR4_PSE (1 << 4)
#define CR4_PGE (1 << 4)
#define CR0_PGENABLE 0x80000000

#define PAGE_SIZE 0x1000

#ifndef ASM_FILE
#define PAGE_ALIGN __attribute__((aligned(PAGE_SIZE)))
#endif

/* sugar to help define pgentry_t's */
#define PAGE_ENTRY(addr, flags) (pgentry_t)((unsigned)addr | (flags))

#define PGADDR(addr) (pgaddr_t)(addr - (addr % PAGE_SIZE))

/* A PDE maps a 4m block (2^22 = 4m) */
#define ADDR_PDE(addr) ((addr) >> 22)
/* A PTE maps a 4k block (2^12 = 4k) */
#define ADDR_PTE(addr) (((addr) >> 12) % 1024)
/* Extracts the address from a PDE */
#define PDE_ADDR(pde) ((pde)&0xFFC00000)
/* Extracts the address from a PTE */
#define PTE_ADDR(pte) ((pte)&0xFFFFF000)

#ifndef ASM_FILE
extern pgentry_t KernelPageDir[1024];

void pg_init(void);
void pg_map_sequence(pgaddr_t paddr, vaddr_t vaddr, size_t length);
void pg_map(pgaddr_t paddr, vaddr_t vaddr);
void pg_unmap(vaddr_t vaddr);
void pg_reserve(vaddr_t vaddr);
void pg_map_ext(pgaddr_t paddr, vaddr_t vaddr, uint32_t flags);
void pg_unmap_ext(vaddr_t vaddr);
pgentry_t *pg_tmp_map(paddr_t addr);
void pg_tmp_unmap(pgentry_t *mapping);
bool pg_is_allocated(vaddr_t addr);
bool pg_is_reserved(vaddr_t addr);

uint32_t mmu_read_cr0(void);
void mmu_write_cr0(uint32_t cr0);
vaddr_t mmu_read_cr2(void);
pgentry_t mmu_read_cr3(void);
void mmu_write_cr3(pgentry_t cr3);
uint32_t mmu_read_cr4(void);
void mmu_write_cr4(uint32_t cr4);

void tlb_flush(void);
void tlb_invlpg(vaddr_t addr);
#endif
