ARCH_PATH := arch/$(ARCH)/

CFLAGS  += -march=i686 -m32
LDFLAGS += -melf_i386

OUT_IMG   := boot.img
CLEANOBJS += $(OUT_IMG)

SOURCES += $(addprefix $(ARCH_PATH), \
                vga_console.c        \
                console.c            \
                syscall.c            \
                io.c)

include arch/$(ARCH)/boot/makefile.mk
include arch/$(ARCH)/mem/makefile.mk
include arch/$(ARCH)/intr/makefile.mk
include arch/$(ARCH)/proc/makefile.mk

boot-nogrub: $(OUT)
	qemu-system-i386 -kernel $(OUT)

boot: $(OUT_IMG)
	qemu-system-i386 -hda $(OUT_IMG)

debug: $(OUT_IMG) $(OUT_SYMS)
	$(eval tmpsh := $(shell mktemp --suffix .gdb))
	echo "symbol-file $(OUT_SYMS)" >> $(tmpsh)
	echo "target remote localhost:1234" >> $(tmpsh)
	$(info )
	$(info gdb --command=$(tmpsh))
	$(info )
	qemu-system-i386 -S -s -hda $(OUT_IMG) &
	sleep 2
	gdb --command=$(tmpsh)

$(OUT_IMG): $(ARCH_PATH)/boot.sfdisk $(OUT) $(INITRD_OUT)
	sudo $(ARCH_PATH)/bin/mkbootimg.sh $@ $(OUT) $(INITRD_OUT)
	sudo chown `whoami` $(OUT_IMG)
