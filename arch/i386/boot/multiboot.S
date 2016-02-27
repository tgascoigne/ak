#define	ASM_FILE 1
#include <arch/i386/boot/multiboot2.h>

# define AOUT_KLUDGE 0

			  .section .multiboot
              .align 8
multiboot_header:
              .long MULTIBOOT2_HEADER_MAGIC
              .long MULTIBOOT_ARCHITECTURE_I386
              .long multiboot_header_end - multiboot_header
              .long -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + (multiboot_header_end - multiboot_header))
multiboot_header_end:
