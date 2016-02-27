#include "early_malloc.h"

#include <stdbool.h>

#include <mem/types.h>
#include <mem/mmu_inline.h>
#include <mem/brk.h>

static bool EarlyMallocEnabled = true;
static vaddr_t AllocStack[0x1000];
static int AllocSP = 0;

extern void *__real_malloc(size_t size);
extern void __real_free(void *addr);

#define enter_kernel()                \
	if (CurrentTask != &KernelTask) { \
		UserTask    = CurrentTask;    \
		CurrentTask = &KernelTask;    \
	}

#define leave_kernel()          \
	if (UserTask) {             \
		CurrentTask = UserTask; \
	}

void *__wrap_malloc(size_t size) {
	if (EarlyMallocEnabled) {
		return early_malloc(size);
	}
	enter_kernel();
	void *mem = __real_malloc(size);
	leave_kernel();
	return mem;
}

void __wrap_free(void *addr) {
	if (EarlyMallocEnabled) {
		early_free(addr);
	}
	enter_kernel();
	__real_free(addr);
	leave_kernel();
}

/* disables early_malloc as the malloc implementation, and switches
   to libc malloc */
void early_malloc_disable(void) {
	EarlyMallocEnabled = false;
}

void *early_malloc(size_t size) {
	vaddr_t brk = task_brk(CurrentTask, 0);
	brk         = task_brk(CurrentTask, brk + size);

	extern void *__curbrk;
	__curbrk = (void *)CurrentTask->brk;

	AllocStack[++AllocSP] = (vaddr_t)brk;
	return (void *)(brk - size);
}

void early_free(void *addr) {
	if (AllocStack[AllocSP] == (vaddr_t)addr) {
		task_brk(CurrentTask, AllocStack[AllocSP--]);

		extern void *__curbrk;
		__curbrk = (void *)CurrentTask->brk;
	}
}
