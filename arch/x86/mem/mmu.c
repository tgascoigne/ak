#include "mmu.h"

#include <arch/x86/mem/framealloc.h>
#include <arch/x86/mem/map.h>
#include <arch/x86/intr/idt.h>
#include <kernel/panic.h>
#include <kernel/proc/task.h>

#include <string.h>

#define USER_FLAGS PAGE_PRESENT | PAGE_RW
#define KERNEL_FLAGS USER_FLAGS | PAGE_LINK
#define KERNEL_LOW_4M PAGE_ENTRY(0, KERNEL_FLAGS | PAGE_EXTENDED | PAGE_GLOBAL)

static pgentry_t NilPgEnt = (pgentry_t)0;

// boot_pgd is the initial page directory used during startup
pgentry_t KernelPageDir[1024] PAGE_ALIGN = {
	[ADDR_PDE(0)] = KERNEL_LOW_4M, [ADDR_PDE(KZERO)] = KERNEL_LOW_4M,
};

pgentry_t TmpPageTbl[1024] PAGE_ALIGN = {0};

void pg_fault_handler(isrargs_t *regs);

void pg_init(void) {
	idt_set_handler(INT_PAGE_FAULT, pg_fault_handler);

	// unmap the kernel from 0x0
	KernelPageDir[ADDR_PDE(0)] = NilPgEnt;
#pragma message("temporarily disabled - breaks interrupts?")

	// point the last 4m at the temp page table
	KernelPageDir[ADDR_PDE(KTMPMEM)] = PAGE_ENTRY(KBSSTOPHYS(&TmpPageTbl), KERNEL_FLAGS);

	KernelTask.pgd = KernelPageDir;

	pg_flush_tlb();
}

static uint32_t base_flags() {
	uint32_t flags = USER_FLAGS;
	if (CurrentTask->pid == KERNEL_PID) {
		flags = KERNEL_FLAGS;
	}
	return flags;
}

void pg_map_sequence(pgaddr_t paddr, vaddr_t vaddr, size_t length) {
	for (size_t i = 0; i < length; i += PAGE_SIZE) {
		pg_map(paddr + (pgaddr_t)i, vaddr + (vaddr_t)i);
	}
}

void pg_map(pgaddr_t paddr, vaddr_t vaddr) {
	uint32_t flags = base_flags();
	pg_map_ext(paddr, vaddr, flags);
}

void pg_reserve(vaddr_t vaddr) {
	uint32_t flags = base_flags();
	flags |= PAGE_RESERVED;
	flags &= (uint32_t)~PAGE_PRESENT; /* reserved pages are not present (allocated on page fault) */
	pg_map_ext(0, vaddr, flags);
}

void pg_map_ext(pgaddr_t paddr, vaddr_t vaddr, uint32_t flags) {
	pgentry_t *dirent = &CurrentTask->pgd[ADDR_PDE(vaddr)];
	if (*dirent == NilPgEnt) {
		uint32_t ptflags = base_flags();

		/* table isn't mapped - allocate a new one */
		paddr_t tblframe = frame_alloc();
		frame_set(tblframe);
		pgentry_t *table = pg_tmp_map((pgaddr_t)tblframe);
		memset(table, 0, (size_t)sizeof(pgentry_t) * 1024);
		pg_tmp_unmap(table);
		*dirent = PAGE_ENTRY(tblframe, ptflags);
	}

	paddr_t tblframe = PDE_ADDR(*dirent);
	pgentry_t *table = pg_tmp_map((pgaddr_t)tblframe);

	pgentry_t *entry = &table[ADDR_PTE(vaddr)];
	*entry	   = PAGE_ENTRY(paddr, flags);

	pg_tmp_unmap(table);
	pg_invlpg(vaddr);
}

static int pg_free_tmp_map() {
	for (int i = 0; i < 1024; i++) {
		if (TmpPageTbl[i] == 0) {
			return i;
		}
	}
	return -1;
}

pgentry_t *pg_tmp_map(pgaddr_t addr) {
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

bool pg_is_allocated(vaddr_t addr) {
	pgentry_t pgd = CurrentTask->pgd[ADDR_PDE(addr)];
	if (pgd == NilPgEnt) {
		return false;
	}

	if ((pgd & PAGE_PRESENT) == 0) {
		return false;
	}

	if ((pgd & PAGE_EXTENDED) != 0) {
		// page is extended and present. we have no pte to examine.
		return true;
	}

	pgentry_t *pt = pg_tmp_map(PDE_ADDR(pgd));
	pgentry_t pte = pt[ADDR_PTE(addr)];
	pg_tmp_unmap(pt);

	if (pte == NilPgEnt) {
		return false;
	}

	if ((pte & PAGE_PRESENT) == 0) {
		return false;
	}

	return true;
}

bool pg_is_reserved(vaddr_t addr) {
	pgentry_t pgd = CurrentTask->pgd[ADDR_PDE(addr)];
	if (pgd == NilPgEnt) {
		return false;
	}

	if ((pgd & PAGE_RESERVED) != 0 && (pgd & PAGE_EXTENDED) != 0) {
		/* 4m is reserved */
		return true;
	}

	if ((pgd & PAGE_EXTENDED) != 0) {
		// page is extended and not reserved. we have no pte to examine.
		return false;
	}

	pgentry_t *pt = pg_tmp_map(PDE_ADDR(pgd));
	pgentry_t pte = pt[ADDR_PTE(addr)];
	pg_tmp_unmap(pt);

	if (pte == NilPgEnt) {
		return false;
	}

	if ((pte & PAGE_RESERVED) != 0) {
		return true;
	}

	return false;
}

void pg_fault_handler(isrargs_t *regs) {
	vaddr_t fault_addr;
	__asm__ volatile("mov %%cr2, %0" : "=r"(fault_addr));

	if (pg_is_reserved(fault_addr)) {
		paddr_t frame = frame_alloc();
		frame_set(frame);
		pg_map(frame, fault_addr);
		return;
	}

	printf("Page fault on unreserved page %02x\n", fault_addr);
}
