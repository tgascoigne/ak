			  .global gdt_load
gdt_load:	  mov	4(%esp), %eax
			  lgdt	(%eax)

			  mov	$0x10, %eax
			  mov	%ax, %ds
			  mov	%ax, %es
			  mov	%ax, %fs
			  mov	%ax, %gs
			  mov	%ax, %ss

			  ljmp	$0x08, $gdt_load_end
gdt_load_end: ret
