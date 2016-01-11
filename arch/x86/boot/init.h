#pragma once

#include <stdbool.h>

#include <arch/x86/boot/multiboot.h>

bool multiboot_check(unsigned long magic, multiboot_info_t* mb_info);
