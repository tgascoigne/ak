#include "kmain.h"

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <syscall.h>
#include <mem/map.h>
#include <mem/brk.h>
#include <mem/early_malloc.h>
#include <intr/intr.h>
#include <kernel/module.h>
#include <kernel/initrd.h>
#include <kernel/panic.h>
#include <kernel/syscall/fdio.h>
#include <kernel/syscall/mem.h>
#include <kernel/syscall/proc.h>
#include <kernel/syscall/syscall.h>
#include <kernel/syscall/signal.h>
#include <kernel/syscall/thread.h>
#include <kernel/syscall/uname.h>
#include <kernel/syscall/user.h>
#include <kernel/fs/node.h>
#include <kernel/proc/sched.h>
#include <kernel/proc/elf/read_elf.h>
#include <kernel/proc/elf/exec.h>
#include <kernel/tty/console.h>

typedef int (*mainfunc_t)(int, char **, char **);

void __uClibc_main(mainfunc_t main, int argc, char **argv, void (*app_init)(void), void (*app_fini)(void),
                   void (*rtld_fini)(void), void *stack_end);

void kmain(int argc, char **argv, char **envp);

void kinit(void) {
	syscall_arch_init();
	mem_syscall_init();
	proc_init();
	thread_init();
	signal_init();
	fdio_init();
	sched_init();
	fs_create_rootfs();
	initrd_mount(FSRootNode);
	uname_syscall_init();
	user_syscall_init();

	if (!modules_init()) {
		PANIC("modules_init failed\n");
		return;
	}

	__uClibc_main((mainfunc_t)kmain, /* main */
	              0,                 /* argc */
	              NULL,              /* argv */
	              NULL,              /* app_init */
	              NULL,              /* app_fini */
	              NULL,              /* rtld_fini */
	              NULL /* stack_end */);
}

void kmain(int argc, char **_argv, char **_envp) {
	early_malloc_disable();
	printf("init complete\n");
	console_init();
	task_unmask_preempt();
	intr_enable();
	/*
	    FILE *fd = fopen("/initrd/hello_world.txt", "r");
	    char buf[256];
	    size_t len = fread(buf, 1, 256, fd);
	    fclose(fd);
	    fwrite(buf, 1, len, stdout);
	*/
	printf("Booting...\n");
	if (fork()) {
		while (1) {
		}
	}

	char *argv[] = {"/initrd/bin/busybox", "ash", NULL};
	char *envp[] = {"foo=bar", NULL};
	execve("/initrd/bin/busybox", argv, envp);
}
