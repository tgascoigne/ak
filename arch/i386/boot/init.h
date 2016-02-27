#pragma once

#include <stdbool.h>

#include <arch/i386/boot/multiboot2.h>
#include <arch/i386/mem/map.h>

void arch_init(vaddr_t mb_info);
