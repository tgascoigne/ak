#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <arch/i386/boot/multiboot2.h>
#include <arch/i386/mem/map.h>

bool multiboot_validate(uint32_t magic, paddr_t mb_info);
void multiboot_mmap(paddr_t mb_info);
