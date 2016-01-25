#include "kmain.h"

#include <stdio.h>
#include <string.h>

#include <arch/x86/mem/map.h>
#include <arch/x86/mem/brk.h>
#include <arch/x86/intr/intr.h>
#include <kernel/module.h>

void kmain(void) {
	if (!modules_init()) {
		fprintf(stderr, "modules_init failed\n");
		return;
	}

	intr_enable();

	task_brk(CurrentTask, KZERO + 0x400000 + (0x4000 * 4));
	char *x = (char *)KZERO + 0x400000;
	memset(x, 0, 0x4000 * 4);

	task_brk(CurrentTask, KZERO + 0x400000);
}
