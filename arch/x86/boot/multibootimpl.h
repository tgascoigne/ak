#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <arch/x86/boot/multiboot2.h>
#include <arch/x86/mem/map.h>

bool multiboot_validate(uint32_t magic, vaddr_t mb_info);
void multiboot_mmap(vaddr_t mb_info);
