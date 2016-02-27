#pragma once

#include <arch/i386/intr/idt.h>

void pic_init(void);
void pic_remap(void);
void pic_eoi(isrargs_t *args);
void pic_mask(uint8_t irq);
void pic_unmask(uint8_t irq);
