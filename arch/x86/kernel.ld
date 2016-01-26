#define LD_SCRIPT 1
#include <arch/x86/mem/map.h>

ENTRY(_start)
OUTPUT_FORMAT("elf32-i386")

SECTIONS
{
    . = KTEXT;

    .text ALIGN(4K) : AT(ADDR(.text) - KZERO)
    {
        *(.multiboot)
        *(.text)
    }

    .modinit : AT(ADDR(.modinit) - KZERO)
    {
        _modinit_start = .;
        *(SORT(.modinit*))
        _modinit_end = .;
    }

    .modexit : AT(ADDR(.modexit) - KZERO)
    {
        _modexit_start = .;
        *(SORT(.modexit*))
        _modexit_end = .;
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - KZERO)
    {
        *(.rodata)
    }

    .data ALIGN(4K) : AT(ADDR(.data) - KZERO)
    {
        *(.data)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - KZERO)
    {
        *(COMMON)
        *(.bss)
    }

    _kern_end = .;
}
