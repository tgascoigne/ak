#pragma once

#include <kernel/proc/elf/exec.h>

int arch_exec(startfunc_t start, char *const argv[], char *const envp[]);
