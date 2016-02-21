#pragma once

#include <stdlib.h>

typedef void (*consbufputc_t)(char c);

void kbd_init(consbufputc_t putc);
ssize_t kbd_gets(void *buf, size_t count);
