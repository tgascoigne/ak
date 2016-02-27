#include "brk.h"

#include <arch/i386/mem/mmu.h>

vaddr_t task_brk(task_t *task, vaddr_t brk) {
	vaddr_t cur = task->brk;

	if (brk == 0) {

	} else if (brk > cur) {
		task->brk = brk;

		for (; cur < brk + PAGE_SIZE; cur += PAGE_SIZE) {
			if (mmu_is_ready() && !pg_is_allocated(cur)) {
				pg_alloc(cur);
			}
		}

	} else if (brk < cur) {
		task->brk = brk;

		for (pgaddr_t i = PGADDR(brk) + 1; i < PGADDR(cur); i += PAGE_SIZE) {
			if (mmu_is_ready()) {
				pg_unmap(i);
			}
		}
	}

	return task->brk;
}
