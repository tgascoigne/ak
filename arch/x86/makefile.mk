ARCH_PATH := arch/$(ARCH)/

ARCH_CFLAGS   := -m32
ARCH_LDFLAGS  :=
BUILDROOT     := /opt/i386-buildroot-akos
CROSS_COMPILE := $(BUILDROOT)/usr/bin/i586-buildroot-linux-uclibc-

OUT_IMG     := boot.img
OUT_ISO     := boot.iso
CLEANOBJS   += $(OUT_IMG) $(OUT_ISO)

SOURCES += $(addprefix $(ARCH_PATH), \
                vga_console.c        \
                console.c            \
                syscall.c            \
                kbd.c                \
                io.c)

include arch/$(ARCH)/boot/makefile.mk
include arch/$(ARCH)/mem/makefile.mk
include arch/$(ARCH)/intr/makefile.mk
include arch/$(ARCH)/proc/makefile.mk

boot-nogrub: $(OUT)
	qemu-system-i386 -kernel $(OUT)

boot: $(OUT_ISO)
	qemu-system-i386 -cdrom $(OUT_ISO)

debug: $(OUT_ISO) $(OUT_SYMS)
	$(eval tmpsh := $(shell mktemp --suffix .gdb))
	echo "symbol-file $(OUT_SYMS)" >> $(tmpsh)
	echo "target remote localhost:1234" >> $(tmpsh)
	$(info )
	$(info gdb --command=$(tmpsh))
	$(info )
	qemu-system-i386 -S -s -cdrom $(OUT_ISO) &
	sleep 2
	gdb --command=$(tmpsh)

$(OUT_IMG): $(ARCH_PATH)/boot.sfdisk $(OUT) $(INITRD_OUT)
	sudo $(ARCH_PATH)/bin/mkbootimg.sh $@ $(OUT) $(INITRD_OUT)
	sudo chown `whoami` $(OUT_IMG)

$(OUT_ISO): $(OUT) $(INITRD_OUT)
	$(ARCH_PATH)/bin/mkbootiso.sh $@ $(OUT) $(INITRD_OUT)
