#define	ASM_FILE 1
#include <arch/i386/mem/map.h>
#include <arch/i386/mem/mmu.h>
#undef ASM_FILE

              .section .text

              .extern multiboot_validate

              .global _start_vaddr

              .equ  _start, (_start_vaddr - KZERO)
              .global _start

_start_vaddr: movl  $KernelPageDir, %ecx
              sub   $KZERO, %ecx
              movl  %ecx, %cr3

              movl  %cr4, %ecx
              orl   $CR4_PSE, %ecx
              orl   $CR4_PGE, %ecx
              movl  %ecx, %cr4

              movl  %cr0, %ecx
              orl   $CR0_PGENABLE, %ecx
              movl  %ecx, %cr0

# Shift the stack pointers up to the virtual mapping
              add   $KZERO, %esp
              add   $KZERO, %ebp
              add   $KZERO, %edi
              add   $KZERO, %esi

              pushl $_start_pgenable
              ret

_start_pgenable:
              movl  $_stack_bottom, %ecx
              sub   $KZERO, %ecx
              movl  %ecx, %esp

              pushl %ebx            # multiboot_info_t*
              pushl %eax            # multiboot magic

              call  multiboot_validate
              add   $4, %esp

              cmp   $0, %eax
              je    error

              call  arch_init

              call  kinit

loop:		  hlt
			  jmp	loop



error:		  pushl $error_str
			  call printf
			  jmp loop

error_str:	  .asciz "boot failed\n"
