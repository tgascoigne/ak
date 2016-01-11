#define	ASM_FILE 1
#include <arch/x86/mem/map.h>
#include <arch/x86/mem/mmu.h>
#undef ASM_FILE

              .section .text

              .extern multiboot_validate

              .global _start_vaddr

              .equ  _start, (_start_vaddr - KZERO)
              .global _start

_start_vaddr: movl  $boot_pgdir, %ecx
              sub   $KZERO, %ecx
              mov   %ecx, %cr3

              mov   %cr4, %ecx
              orl   $CR4_PSE, %ecx
              mov   %ecx, %cr4

              mov   %cr0, %ecx
              orl   $CR0_PGENABLE, %ecx
              mov   %ecx, %cr0

              push $_start_pgenable
              ret

_start_pgenable:
              push  %ebx            # multiboot_info_t*
              push  %eax            # multiboot magic

              call  multiboot_validate
              add   $8, %esp

              cmp   $0, %eax
              je   error



loop:		  hlt
			  jmp	loop

error:		  pushl $error_str
			  call printf
			  jmp loop

error_str:	  .asciz "boot failed\n"

#              .section .data
#              .align 0x1000
#boot_pgd:     .word (PAGE_PRESENT | PAGE_RW | PAGE_EXTENDED)
#              .fill (ADDR_PDE(KZERO) - 1), 4, 0
#boot_pgd_2:   .word (PAGE_PRESENT | PAGE_RW | PAGE_EXTENDED)
#              .fill (ADDR_PDE(MEMMAX) - ADDR_PDE(KZERO) - 1), 4, 0
