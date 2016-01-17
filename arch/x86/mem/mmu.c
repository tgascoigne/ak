#include "mmu.h"

#include <arch/x86/mem/map.h>
#include <kernel/panic.h>
#include <kernel/proc/task.h>

#define KERNEL_FLAGS PAGE_PRESENT | PAGE_RW | PAGE_LINK
#define KERNEL_LOW_4M PAGE_ENTRY(0, KERNEL_FLAGS | PAGE_EXTENDED)

static pgentry_t NilPgEnt = (pgentry_t)0;

// boot_pgd is the initial page directory used during startup
pgentry_t KernelPageDir[1024] PAGE_ALIGN = {
	[ADDR_PDE(0)] = KERNEL_LOW_4M, [ADDR_PDE(KZERO)] = KERNEL_LOW_4M,
};

pgentry_t TmpPageTbl[1024] PAGE_ALIGN = {0};

void pg_init(void) {
	// unmap the kernel from 0x0
	KernelPageDir[ADDR_PDE(0)] = NilPgEnt;

	// point the last 4m at the temp page table
	KernelPageDir[ADDR_PDE(KTMPMEM)] = PAGE_ENTRY(KBSSTOPHYS(&TmpPageTbl), KERNEL_FLAGS);

	KernelTask.pgd = KernelPageDir;

	pg_flush_tlb();
}

static int pg_free_tmp_map() {
	for (int i = 0; i < 1024; i++) {
		if (TmpPageTbl[i] == 0) {
			return i;
		}
	}
	return -1;
}

pgentry_t *pg_tmp_map(paddr_t addr) {
	int entry = pg_free_tmp_map();
	if (entry == -1) {
		PANIC("Out of free temporary mapping pages");
	}

	vaddr_t tmpaddr   = KTMPMEM + ((vaddr_t)entry * PAGE_SIZE);
	TmpPageTbl[entry] = PAGE_ENTRY(addr, KERNEL_FLAGS);

	pg_invlpg(tmpaddr);

	return (pgentry_t *)tmpaddr;
}

void pg_tmp_unmap(pgentry_t *mapping) {
	vaddr_t tmpaddr = (vaddr_t)mapping;
	TmpPageTbl[ADDR_PTE(tmpaddr)] = NilPgEnt;

	pg_invlpg(tmpaddr);
}

void pg_flush_tlb(void) {
	__asm__ volatile("movl %%cr3, %%eax\n\t"
			 "movl %%eax, %%cr3\n\t"
			 :
			 :
			 : "%eax");
}

void pg_invlpg(vaddr_t addr) {
	__asm__ volatile("invlpg (%0)" : : "b"(addr) : "memory");
}
