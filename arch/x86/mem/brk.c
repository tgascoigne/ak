#include "brk.h"

#include <arch/x86/mem/mmu.h>

vaddr_t task_brk(task_t *task, vaddr_t brk) {
	vaddr_t cur = task->brk;

	if (brk == 0) {

	} else if (brk > cur) {
		task->brk = brk;

		for (; cur < brk; cur += PAGE_SIZE) {
			if (mmu_is_ready() && !pg_is_allocated(cur)) {
				pg_reserve(cur);
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
