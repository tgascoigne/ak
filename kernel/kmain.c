#include "kmain.h"

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <syscall.h>
#include <mem/map.h>
#include <mem/brk.h>
#include <intr/intr.h>
#include <kernel/module.h>
#include <kernel/initrd.h>
#include <kernel/panic.h>
#include <kernel/syscall/fdio.h>
#include <kernel/syscall/mem.h>
#include <kernel/syscall/proc.h>
#include <kernel/syscall/syscall.h>
#include <kernel/syscall/uname.h>
#include <kernel/fs/node.h>
#include <kernel/proc/sched.h>
#include <kernel/proc/elf/read_elf.h>
#include <kernel/proc/elf/exec.h>
#include <kernel/tty/console.h>

void kmain(void) {
	syscall_arch_init();
	mem_syscall_init();
	proc_init();
	fdio_init();
	sched_init();
	fs_create_rootfs();
	initrd_mount(FSRootNode);
	uname_syscall_init();

	if (!modules_init()) {
		PANIC("modules_init failed\n");
		return;
	}

	console_init();

	printf("module init complete\n");

	task_unmask_preempt();
	intr_enable();

	if (fork()) {
		FILE *fd = fopen("/initrd/hello_world.txt", "r");
		char buf[256];
		size_t len = fread(buf, 1, 256, fd);
		fclose(fd);
		fwrite(buf, 1, len, stdout);
	} else {
		char *argv[] = {"initrd/hello", NULL};
		char *envp[] = {"foo=bar", NULL};
		execve("/initrd/hello", argv, envp);
	}
}
