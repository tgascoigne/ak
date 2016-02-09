#define	ASM_FILE 1
#include <arch/x86/mem/map.h>
#include <arch/x86/mem/mmu.h>
#undef ASM_FILE

              .section .text

              .extern multiboot_validate

              .global _start

_start:       movl  $KernelPageDir, %ecx
              sub   $KZERO, %ecx
              mov   %ecx, %cr3

              mov   %cr4, %ecx
              orl   $CR4_PSE, %ecx
              orl   $CR4_PGE, %ecx
              mov   %ecx, %cr4

              mov   %cr0, %ecx
              orl   $CR0_PGENABLE, %ecx
              mov   %ecx, %cr0

              # Shift the stack pointers up to the virtual mapping
              add   $KZERO, %esp
              add   $KZERO, %ebp
              add   $KZERO, %edi
              add   $KZERO, %esi

              push $_start_pgenable
              ret

_start_pgenable:
              movl  $_stack_bottom, %ecx
              sub   $KZERO, %ecx
              movl  %ecx, %esp

              add   $KZERO, %ebx    # need to adjust the pointer for the new zero addr
              push  %ebx            # multiboot_info_t*
              push  %eax            # multiboot magic

              call  multiboot_validate
              add   $4, %esp

              cmp   $0, %eax
              je   error

              call  arch_init

              call  kmain

loop:		  hlt
			  jmp	loop

error:		  pushl $error_str
			  call printf
			  jmp loop

error_str:	  .asciz "boot failed\n"
