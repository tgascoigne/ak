#include "mmu.h"

void tlb_flush(void) {
	mmu_write_cr3(mmu_read_cr3());
}

void tlb_invlpg(vaddr_t addr) {
	__asm__ volatile("invlpg (%0)" : : "b"(addr) : "memory");
}
