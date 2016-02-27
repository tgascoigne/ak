#pragma once

#include <mem/types.h>

#include <stdbool.h>
#include <stdint.h>

void frame_alloc_init(paddr_t mem_max);
void frame_clear(paddr_t ptr);
void frame_set(paddr_t ptr);
void frame_clear_range(paddr_t begin, paddr_t end);
void frame_set_range(paddr_t begin, paddr_t end);
bool frame_test(paddr_t ptr);
paddr_t frame_alloc(void);
paddr_t frame_alloc_4m(void);
paddr_t frame_alloc_after(paddr_t addr);
