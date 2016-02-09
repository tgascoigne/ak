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
#include <kernel/fs/node.h>
#include <kernel/proc/sched.h>
#include <kernel/proc/read_elf.h>
#include <kernel/tty/console.h>

void kmain(void) {
	syscall_arch_init();
	mem_syscall_init();
	proc_init();
	fdio_init();
	sched_init();
	fs_create_rootfs();
	initrd_mount(FSRootNode);

	if (!modules_init()) {
		PANIC("modules_init failed\n");
		return;
	}

	console_init();

	printf("module init complete\n");

	task_unmask_preempt();
	intr_enable();

	FILE *fd = fopen("/initrd/hello_world.txt", "r");
	char buf[256];
	size_t len = fread(buf, 1, 256, fd);
	fclose(fd);
	fwrite(buf, 1, len, stdout);

	elf_t init;
	if (!elf_read("/initrd/hello", &init)) {
		PANIC("couldn't parse init\n");
	}

	elf_print_sections(&init);
}
