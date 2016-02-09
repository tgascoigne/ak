#pragma once

#include <stdbool.h>

#include <arch/x86/boot/multiboot2.h>
#include <arch/x86/mem/map.h>

void arch_init(vaddr_t mb_info);
