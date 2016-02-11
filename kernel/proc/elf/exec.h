#pragma once

#include <kernel/proc/elf/read_elf.h>

typedef int (*startfunc_t)();

void elf_load(elf_t *elf);
void elf_start(elf_t *elf, char *const argv[], char *const envp[]);
