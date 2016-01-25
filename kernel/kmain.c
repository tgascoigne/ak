#include "kmain.h"

#include <stdio.h>
#include <stdlib.h>
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

	char *x = malloc(0x4000 * 4);
	memset(x, 0, 0x4000 * 4);
	free(x);
}
