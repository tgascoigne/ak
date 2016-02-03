#include "kmain.h"

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arch/x86/mem/map.h>
#include <arch/x86/mem/brk.h>
#include <arch/x86/intr/intr.h>
#include <kernel/module.h>

void kmain(void) {
	if (!modules_init()) {
		fprintf(stderr, "modules_init failed\n");
		return;
	}

	printf("module init complete\n");

	task_unmask_preempt();
	intr_enable();

	if (fork()) {
		while (1) {
			printf("foo %i\n", getpid());
		}
	} else {
		while (1) {
			printf("bar %i\n", getpid());
		}
	}
}
