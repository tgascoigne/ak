#include "mmu.h"

uint32_t mmu_read_cr0(void) {
	uint32_t cr0;
	__asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
	return cr0;
}

void mmu_write_cr0(uint32_t cr0) {
	__asm__ volatile("mov %0, %%cr0" : : "b"(cr0));
}

vaddr_t mmu_read_cr2(void) {
	vaddr_t fault_addr;
	__asm__ volatile("mov %%cr2, %0" : "=r"(fault_addr));
	return fault_addr;
}

pgentry_t mmu_read_cr3(void) {
	pgentry_t cr3;
	__asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
	return cr3;
}

void mmu_write_cr3(pgentry_t cr3) {
	__asm__ volatile("mov %0, %%cr3" : : "b"(cr3));
}

uint32_t mmu_read_cr4(void) {
	uint32_t cr4;
	__asm__ volatile("mov %%cr4, %0" : "=r"(cr4));
	return cr4;
}

void mmu_write_cr4(uint32_t cr4) {
	__asm__ volatile("mov %0, %%cr4" : : "b"(cr4));
}
