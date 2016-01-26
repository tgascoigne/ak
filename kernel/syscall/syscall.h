#pragma once

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define NUM_SYSCALLS 0xFF

typedef uint32_t (*syscall_fn_t)();

extern int syscall_argc[NUM_SYSCALLS];
extern syscall_fn_t syscall_funcs[NUM_SYSCALLS];

#define SYS_READ 0x03
#define SYS_WRITE 0x04
#define SYS_OPEN 0x05
#define SYS_CLOSE 0x06
#define SYS_BRK 0x2d
#define SYS_MMAP 0x5a

void syscall_register(int syscall, syscall_fn_t fn, int argc);
