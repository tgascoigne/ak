#include "mmu.h"

#include <arch/x86/mem/framealloc.h>
#include <arch/x86/mem/map.h>
#include <arch/x86/intr/idt.h>
#include <kernel/panic.h>
#include <kernel/proc/task.h>

#include <string.h>

#define BASE_FLAGS PAGE_PRESENT | PAGE_RW
#define USER_FLAGS BASE_FLAGS | PAGE_USER
#define KERNEL_FLAGS BASE_FLAGS | PAGE_LINK
#define KERNEL_LOW_4M PAGE_ENTRY(0, KERNEL_FLAGS | PAGE_EXTENDED)

// boot_pgd is the initial page directory used during startup
pgentry_t KernelPageDir[1024] PAGE_ALIGN = {
	[ADDR_PDE(0)] = KERNEL_LOW_4M, [ADDR_PDE(KZERO)] = KERNEL_LOW_4M,
};
pgentry_t TmpPageTbl[1024] PAGE_ALIGN     = {0};
pgentry_t KernelStackTbl[1024] PAGE_ALIGN = {0};
static bool MMUReady		       = false;

void pg_fault_handler(isrargs_t *regs);

void pg_init(void) {
	idt_set_handler(INT_PAGE_FAULT, pg_fault_handler);

	// unmap the kernel from 0x0
	//	KernelPageDir[ADDR_PDE(0)] = NilPgEnt;

	// point the last 4m at the temp page table
	KernelPageDir[ADDR_PDE(KTMPMEM)] = PAGE_ENTRY(KBSSTOPHYS(&TmpPageTbl), KERNEL_FLAGS | PAGE_LINK);

	KernelTask.pgd = (pgaddr_t)KBSSTOPHYS(&KernelPageDir);

#define rebase_sp(x) (KSTACK + (x - KBSSTOPHYS(&_stack_bottom)))

	extern void *_stack_bottom;
	extern void *_stack_top;

#pragma message("extend stack on PAGE_STACK_END fault")
	KernelStackTbl[1022] = PAGE_ENTRY(0, PAGE_STACK_END);
	KernelStackTbl[1023] = PAGE_ENTRY(KBSSTOPHYS(&_stack_top), BASE_FLAGS);
	KernelPageDir[ADDR_PDE(KSTACK - 1)] = PAGE_ENTRY(KBSSTOPHYS(&KernelStackTbl), BASE_FLAGS);

	uint32_t sp, bp;
	__asm__ volatile("mov %%esp, %0" : "=r"(sp));
	__asm__ volatile("mov %%ebp, %0" : "=r"(bp));
	sp = rebase_sp(sp);
	bp = rebase_sp(bp);
	__asm__ volatile("mov %0, %%esp" : : "b"(sp));
	__asm__ volatile("mov %0, %%ebp" : : "b"(bp));

	tlb_flush();
	MMUReady = true;
}

bool mmu_is_ready(void) {
	return MMUReady;
}

static uint32_t base_flags(void) {
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

void pg_unmap(vaddr_t vaddr) {
	pgentry_t *dir    = pg_tmp_map(CurrentTask->pgd);
	pgentry_t *dirent = &dir[ADDR_PDE(vaddr)];
	if (*dirent == NilPgEnt) {
		// nothing to do
		pg_tmp_unmap(dir);
		return;
	}

	if ((*dirent & PAGE_EXTENDED) != 0) {
		// We can't unmap a single page within a 4m range
		// To unmap the entire range, call pg_unmap_ext instead
		pg_tmp_unmap(dir);
		return;
	}

	paddr_t tblframe = PDE_ADDR(*dirent);
	pgentry_t *table = pg_tmp_map((pgaddr_t)tblframe);

	pgentry_t *entry = &table[ADDR_PTE(vaddr)];
	*entry	   = NilPgEnt;

	pg_tmp_unmap(table);
	pg_tmp_unmap(dir);
	tlb_invlpg(vaddr);
}

void pg_reserve(vaddr_t vaddr) {
	uint32_t flags = base_flags();
	flags |= PAGE_RESERVED;
	flags &= (uint32_t)~PAGE_PRESENT; /* reserved pages are not present (allocated on page fault) */
	pg_map_ext(MEMMAX, vaddr, flags);
}

pgaddr_t pg_dir_new(void) {
	paddr_t dirframe = frame_alloc();
	frame_set(dirframe);
	pgentry_t *dir = pg_tmp_map((pgaddr_t)dirframe);
	memset(dir, 0, (size_t)sizeof(pgentry_t) * 1024);
	pg_tmp_unmap(dir);
	return dirframe;
}

void pg_map_ext(pgaddr_t paddr, vaddr_t vaddr, uint32_t flags) {
	pgentry_t *dir    = pg_tmp_map(CurrentTask->pgd);
	pgentry_t *dirent = &dir[ADDR_PDE(vaddr)];
	if (*dirent == NilPgEnt) {
		uint32_t ptflags = base_flags();

		/* table isn't mapped - allocate a new one */
		paddr_t tblframe = pg_dir_new();
		*dirent	  = PAGE_ENTRY(tblframe, ptflags);
	}

	paddr_t tblframe = PDE_ADDR(*dirent);
	pgentry_t *table = pg_tmp_map((pgaddr_t)tblframe);

	pgentry_t *entry = &table[ADDR_PTE(vaddr)];
	*entry	   = PAGE_ENTRY(paddr, flags);

	pg_tmp_unmap(table);
	pg_tmp_unmap(dir);
	tlb_invlpg(vaddr);
}

void pg_unmap_ext(vaddr_t vaddr) {
	pgentry_t *dir    = pg_tmp_map(CurrentTask->pgd);
	pgentry_t *dirent = &dir[ADDR_PDE(vaddr)];
	if (*dirent == NilPgEnt) {
		// nothing to do
		pg_tmp_unmap(dir);
		return;
	}

	*dirent = NilPgEnt;

	tlb_flush();
	pg_tmp_unmap(dir);
}

static int pg_free_tmp_map(void) {
	for (int i = 0; i < 1024; i++) {
		if (TmpPageTbl[i] == 0) {
			return i;
		}
	}
	return -1;
}

void *pg_tmp_map(pgaddr_t addr) {
	int entry = pg_free_tmp_map();
	if (entry == -1) {
		PANIC("Out of free temporary mapping pages\n");
	}

	vaddr_t tmpaddr   = KTMPMEM + ((vaddr_t)entry * PAGE_SIZE);
	TmpPageTbl[entry] = PAGE_ENTRY(addr, KERNEL_FLAGS);

	tlb_invlpg(tmpaddr);

	return (pgentry_t *)tmpaddr;
}

void pg_tmp_unmap(const void *mapping) {
	vaddr_t tmpaddr = (vaddr_t)mapping;
	TmpPageTbl[ADDR_PTE(tmpaddr)] = NilPgEnt;

	tlb_invlpg(tmpaddr);
}

bool pg_is_allocated(vaddr_t addr) {
	pgentry_t *dir = pg_tmp_map(CurrentTask->pgd);
	pgentry_t pgd = dir[ADDR_PDE(addr)];
	pg_tmp_unmap(dir);

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
	pgentry_t *dir = pg_tmp_map(CurrentTask->pgd);
	pgentry_t pgd = dir[ADDR_PDE(addr)];
	pg_tmp_unmap(dir);

	if (pgd == NilPgEnt) {
		return false;
	}

	if ((pgd & PAGE_PRESENT) != 0) {
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
	vaddr_t fault_addr = mmu_read_cr2();

	if (pg_is_reserved(fault_addr)) {
		paddr_t frame = frame_alloc();
		frame_set(frame);
		pg_map(frame, fault_addr);
		return;
	}

	PANIC("Page fault on unreserved page %02x\n", fault_addr);
}
