#include "framealloc.h"

#include <string.h>
#include <stdlib.h>

#include <arch/x86/mem/mmu.h>
#include <kernel/panic.h>

#define BITS_PER_ENTRY (8 * 4) // 8 bits * 4 characters (uint32_t)
#define BIT_IDX(x) (x / BITS_PER_ENTRY)
#define BIT_OFFSET(x) (x % BITS_PER_ENTRY)
#define ADDR_PAGE(x) (x / PAGE_SIZE)

static uint32_t *FrameBitmap;
static uint32_t FrameCount;

void frame_alloc_init(paddr_t mem_max) {
	FrameCount   = ADDR_PAGE(mem_max);
	size_t count = BIT_IDX(FrameCount);
	FrameBitmap = malloc(count * sizeof(uint32_t));
	memset(FrameBitmap, 0, count * sizeof(uint32_t));
}

void frame_clear(paddr_t ptr) {
	uint32_t entry = ADDR_PAGE(ptr);
	uint32_t idx   = BIT_IDX(entry);
	uint32_t off = BIT_OFFSET(entry);
	FrameBitmap[idx] &= ~(1 << off);
}

void frame_set(paddr_t ptr) {
	uint32_t entry = ADDR_PAGE(ptr);
	uint32_t idx   = BIT_IDX(entry);
	uint32_t off = BIT_OFFSET(entry);
	FrameBitmap[idx] |= (1 << off);
}

void frame_clear_range(paddr_t begin, paddr_t end) {
	for (paddr_t i = begin; i < end; i += PAGE_SIZE) {
		frame_clear(i);
	}
}

void frame_set_range(paddr_t begin, paddr_t end) {
	for (paddr_t i = begin; i < end; i += PAGE_SIZE) {
		frame_set(i);
	}
}

bool frame_test(paddr_t ptr) {
	uint32_t entry = (uint32_t)ADDR_PAGE(ptr);
	uint32_t idx   = (uint32_t)BIT_IDX(entry);
	uint32_t off = (uint32_t)BIT_OFFSET(entry);
	return (FrameBitmap[idx] & (1 << off)) != 0;
}

paddr_t frame_alloc(void) {
	return frame_alloc_after((paddr_t)0);
}

paddr_t frame_alloc_4m(void) {
	size_t bytecount = BIT_IDX(FrameCount);
	int f0           = 0;
	/* find a sequence of 8 bytes (4m) = 0 */
	/* 32 frames per byte * 32 = 1024 frames = 4m */
	for (int i = 0; i < 32 && f0 < (int)bytecount; i++) {
		if (FrameBitmap[f0 + i] != 0) {
			i  = 0;
			f0 = f0 + i + 1;
		}
	}
	return (paddr_t)(f0 * BITS_PER_ENTRY * PAGE_SIZE);
}

paddr_t frame_alloc_after(paddr_t addr) {
	for (paddr_t i = ADDR_PAGE(addr); i < FrameCount; i++) {
		if (FrameBitmap[i] == (uint32_t)-1) {
			// -1 = all bits set, nothing free here
			continue;
		}
#pragma message("associate mapped pages with the task structure")
		for (paddr_t j = 0; j < BITS_PER_ENTRY; j++) {
			if ((FrameBitmap[i] & (1 << j)) == 0) {
				return (i * BITS_PER_ENTRY + j) * PAGE_SIZE;
			}
		}
	}
	PANIC("Out of memory\n")
	return 0;
}
