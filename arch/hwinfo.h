#pragma once

#include <arch/x86/types.h>

typedef struct { paddr_t mem_max; } hwinfo_t;

extern hwinfo_t HardwareInfo;
