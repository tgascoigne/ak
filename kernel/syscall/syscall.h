#pragma once

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define NUM_SYSCALLS 0xFF

typedef uint32_t (*syscall_fn_t)();

extern int syscall_argc[NUM_SYSCALLS];
extern syscall_fn_t syscall_funcs[NUM_SYSCALLS];

#define SYS_EXIT 0x01
#define SYS_FORK 0x02
#define SYS_READ 0x03
#define SYS_WRITE 0x04
#define SYS_OPEN 0x05
#define SYS_CLOSE 0x06
#define SYS_EXECVE 0x0b
#define SYS_GETPID 0x14
#define SYS_BRK 0x2d
#define SYS_IOCTL 0x36
#define SYS_FCNTL 0x37
#define SYS_MMAP 0x5a
#define SYS_NEWSTAT 0x6a
#define SYS_WAIT4 0x72
#define SYS_UNAME 0x7a
#define SYS_SCHED_YIELD 0x9e
#define SYS_GETCWD 0xb7
#define SYS_MMAP_PGOFF 0xc0
#define SYS_GETUID 0xc7
#define SYS_GETGID 0xc8
#define SYS_GETEUID 0xc9
#define SYS_GETEGID 0xca
#define SYS_GETPGID 0x84
#define SYS_GETPGRP 0x41
#define SYS_SETPGID 0x39

void syscall_register(int syscall, syscall_fn_t fn, int argc);
