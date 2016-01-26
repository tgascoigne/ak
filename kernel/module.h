#pragma once

#include <stdbool.h>

typedef bool (*modfunc_t)(void);

extern modfunc_t _modinit_start, _modinit_end;
extern modfunc_t _modexit_start, _modexit_end;

#define MODINIT_ARCH "0"
#define MODINIT_MEM "0"
#define MODINIT_FDIO "1"
#define MODINIT_TTY "2"
#define MODINIT_LATE "9"

#define module_init(f) module_init_prio(f, MODINIT_LATE);
#define module_init_prio(f, prio) static modfunc_t _modinit_##f __attribute__((used, section(".modinit$" prio))) = f
#define module_exit(f) static modfunc_t _modexit_##f __attribute__((used, section(".modexit"))) = f

bool modules_init(void);
bool modules_exit(void);
