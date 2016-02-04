#include "console.h"

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <kernel/panic.h>
#include <kernel/fs/node.h>

#define CONSOLE_DEV "/dev/tty"

void console_init(void) {
	/* init fd 0-2 */
	FILE *fd = fopen(CONSOLE_DEV, "w");
	if (!fd) {
		PANIC("Unable to open %s: %s\n", CONSOLE_DEV, strerror(errno));
	}
	fopen(CONSOLE_DEV, "w");
	fopen(CONSOLE_DEV, "w");
}
