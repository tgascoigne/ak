#include "early_malloc.h"

#include <stdbool.h>

#include <arch/x86/types.h>
#include <arch/x86/mem/brk.h>

static bool EarlyMallocEnabled = true;
static vaddr_t AllocStack[0x1000];
static int AllocSP = 0;

extern void *__real_malloc(size_t size);
extern void __real_free(void *addr);

void *__wrap_malloc(size_t size) {
	if (EarlyMallocEnabled) {
		return early_malloc(size);
	}
	return __real_malloc(size);
}

void __wrap_free(void *addr) {
	if (EarlyMallocEnabled) {
		early_free(addr);
	}
	__real_free(addr);
}

/* disables early_malloc as the malloc implementation, and switches
   to libc malloc */
void early_malloc_disable() {
	EarlyMallocEnabled = false;
}

void *early_malloc(size_t size) {
	vaddr_t brk = task_brk(CurrentTask, 0);
	brk	 = task_brk(CurrentTask, brk);

	AllocStack[++AllocSP] = (vaddr_t)brk;
	return (void *)brk;
}

void early_free(void *addr) {
	if (AllocStack[AllocSP] == (vaddr_t)addr) {
		task_brk(CurrentTask, AllocStack[AllocSP--]);
	}
}
