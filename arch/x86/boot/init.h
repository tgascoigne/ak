#pragma once

#include <stdbool.h>

#include <arch/x86/boot/multiboot.h>

void arch_init(multiboot_info_t *mb_info);
