#define LD_SCRIPT 1
#include <arch/i386/mem/map.h>

ENTRY(_start_vaddr)
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

    .preinit_array : {
        __preinit_array_start = .;
/*        *(.preinit_array) */
        __preinit_array_end = .;
    }

    .init_array : {
        __init_array_start = .;
/*        *(.init_array) */
        __init_array_end = .;
    }

    .fini_array : {
        __fini_array_start = .;
/*        *(.fini_array) */
        __fini_array_end = .;
    }

    /* reserve some space for the first stack page */
    . = ALIGN(0x1000);
    _stack_top = .;
    . += 0x1000;
    _stack_bottom = .;

    _kern_end = .;
}
