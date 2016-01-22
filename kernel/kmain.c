#include "kmain.h"

#include <stdio.h>

#include <kernel/module.h>

void kmain(void) {
	if (!modules_init()) {
		fprintf(stderr, "modules_init failed\n");
		return;
	}
	printf("xxx\n");
}
