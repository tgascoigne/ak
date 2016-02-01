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

	intr_enable();

	char *x = malloc(0x4000 * 4);
	memset(x, 0, 0x4000 * 4);
	free(x);

	fork();

	while (1) {
		printf("my pid is %i\n", CurrentTask->pid);
		sched_yield();
	}
}
