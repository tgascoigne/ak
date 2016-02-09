#define	ASM_FILE 1
#include <arch/x86/boot/multiboot.h>
#undef ASM_FILE

#define MULTIBOOT_HEADER_FLAGS (MULTIBOOT_INFO_ALIGN | MULTIBOOT_INFO_MEMORY)

			  .section .multiboot
			  .align 4
			  .long	MULTIBOOT_HEADER_MAGIC
			  .long	MULTIBOOT_HEADER_FLAGS
			  .long	-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
              .long 0, 0, 0, 0, 0
              .long 0
              .long 1024, 768, 32
