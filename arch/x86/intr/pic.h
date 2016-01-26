#pragma once

#include <arch/x86/intr/idt.h>

void pic_init(void);
void pic_remap(void);
void pic_eoi(isrargs_t *args);
