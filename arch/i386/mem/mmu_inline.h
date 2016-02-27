#pragma once

#include <arch/i386/mem/types.h>

#define __mmu_inline__ static inline __attribute__((always_inline))

__mmu_inline__ uint32_t mmu_read_cr0(void) {
	uint32_t cr0;
	__asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
	return cr0;
}

__mmu_inline__ void mmu_write_cr0(uint32_t cr0) {
	__asm__ volatile("mov %0, %%cr0" : : "b"(cr0));
}

__mmu_inline__ vaddr_t mmu_read_cr2(void) {
	vaddr_t fault_addr;
	__asm__ volatile("mov %%cr2, %0" : "=r"(fault_addr));
	return fault_addr;
}

__mmu_inline__ pgaddr_t mmu_read_cr3(void) {
	pgaddr_t cr3;
	__asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
	return cr3;
}

__mmu_inline__ void mmu_write_cr3(pgaddr_t cr3) {
	__asm__ volatile("mov %0, %%cr3" : : "b"(cr3));
}

__mmu_inline__ uint32_t mmu_read_cr4(void) {
	uint32_t cr4;
	__asm__ volatile("mov %%cr4, %0" : "=r"(cr4));
	return cr4;
}

__mmu_inline__ void mmu_write_cr4(uint32_t cr4) {
	__asm__ volatile("mov %0, %%cr4" : : "b"(cr4));
}

__mmu_inline__ void tlb_flush(void) {
	mmu_write_cr3(mmu_read_cr3());
}

__mmu_inline__ void tlb_invlpg(vaddr_t addr) {
	__asm__ volatile("invlpg (%0)" : : "b"(addr) : "memory");
}

__mmu_inline__ void pg_switch_dir(pgaddr_t dirframe) {
	mmu_write_cr3(dirframe);
	tlb_flush();
}
