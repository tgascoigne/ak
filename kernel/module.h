#pragma once

#include <stdbool.h>

typedef bool (*modfunc_t)(void);

extern modfunc_t _modinit_start, _modinit_end;
extern modfunc_t _modexit_start, _modexit_end;

#define module_init(f) static modfunc_t _modinit_##f __attribute__((used, section(".modinit"))) = f
#define module_exit(f) static modfunc_t _modexit_##f __attribute__((used, section(".modexit"))) = f

bool modules_init(void);
bool modules_exit(void);
