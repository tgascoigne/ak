ARCH_PATH := arch/$(ARCH)/

CFLAGS  += -march=i686 -m32
LDFLAGS += -melf_i386

OUT_IMG   := $(addsuffix .img, $(basename $(OUT)))
CLEANOBJS += $(OUT_IMG)

SOURCES += $(addprefix $(ARCH_PATH), \
                vga_console.c        \
                io.c)

include arch/$(ARCH)/boot/makefile.mk
include arch/$(ARCH)/mem/makefile.mk
include arch/$(ARCH)/intr/makefile.mk

boot-nogrub: $(OUT)
	qemu-system-i386 -kernel $(OUT)

boot: $(OUT_IMG)
	qemu-system-i386 -fda $(OUT_IMG)

debug: $(OUT_IMG) $(OUT_SYMS)
	$(eval tmpsh := $(shell mktemp --suffix .gdb))
	echo "symbol-file $(OUT_SYMS)" >> $(tmpsh)
	echo "target remote localhost:1234" >> $(tmpsh)
	$(info )
	$(info gdb --command=$(tmpsh))
	$(info )
	qemu-system-i386 -S -s -fda $(OUT_IMG) &
	sleep 2
	gdb --command=$(tmpsh)

$(OUT_IMG): $(OUT) $(INITRD_OUT)
	cp $(ARCH_PATH)/floppy.img.template $(OUT_IMG)
	mcopy -i $(OUT_IMG) -/ $(ARCH_PATH)/grub/ ::/
	mcopy -i $(OUT_IMG) $(INITRD_OUT) ::/
	mcopy -i $(OUT_IMG) $(OUT) ::/
