#define	ASM_FILE 1
#include <arch/i386/mem/gdt.h>
#undef ASM_FILE

              .global gdt_load
gdt_load:	  mov	4(%esp), %eax
			  lgdt	(%eax)

			  mov	$GDT_KDATA_OFFS, %eax
			  mov	%ax, %ds
			  mov	%ax, %es
			  mov	%ax, %fs
			  mov	%ax, %gs

			  ljmp	$0x08, $gdt_load_end
gdt_load_end: ret
