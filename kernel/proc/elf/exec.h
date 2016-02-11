#pragma once

#include <kernel/proc/elf/read_elf.h>

void elf_load(elf_t *elf);
void elf_start(elf_t *elf);
