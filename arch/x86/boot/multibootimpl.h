#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <arch/x86/boot/multiboot.h>

bool multiboot_validate(uint32_t magic, multiboot_info_t* mb_info);
void multiboot_mmap(multiboot_info_t* mb_info);
