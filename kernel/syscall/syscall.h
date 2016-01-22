#pragma once

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define NUM_SYSCALLS 0xFF

typedef uint32_t (*syscall_fn_t)();

extern int syscall_argc[NUM_SYSCALLS];
extern syscall_fn_t syscall_funcs[NUM_SYSCALLS];

void syscall_register(int syscall, syscall_fn_t fn, int argc);
