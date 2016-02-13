ARCH  ?= x86
BOARD ?= generic

CC		:= $(CROSS_COMPILE)clang
LD		:= $(CROSS_COMPILE)ld
AS		:= $(CROSS_COMPILE)as
OBJDUMP := $(CROSS_COMPILE)objdump
OBJCOPY := $(CROSS_COMPILE)objcopy
PREPROC := $(CC) -E -x c -P

WARNFLAGS   := -Wall -Wextra -Wpedantic -Werror -Wconversion -Wshadow -Wno-unused-parameter -Wno-microsoft

CFLAGS		:= -iwithprefix include $(WARNFLAGS) -fms-extensions -g
LDFLAGS     := -static
LIB_INC     := -L/usr/lib/gcc/x86_64-redhat-linux/5.3.1/32 -LuClibc/lib/
HEADER_INC  := -I. -isystem /usr/i386-linux-uclibc/usr/include/
LIBS		:= -lc -lgcc
LD_SCRIPT   := arch/$(ARCH)/kernel.ld
OUT			:= kernel.elf
OUT_SYMS    := $(addsuffix .sym, $(basename $(OUT)))
INITRD_OUT  := initrd.img
INITRD_DIR  := initrd

SOURCES   :=
OBJECTS   :=
ASFLAGS   :=
DISASM    :=
BINFILE   :=
CLEANOBJS :=

all: $(OUT) $(DISASM) $(BINFILE)

include arch/makefile.mk
include kernel/makefile.mk
include userspace/makefile.mk

OBJECTS   += $(addsuffix .o, $(basename $(SOURCES)))
DISASM    += $(addsuffix .dis, $(basename $(OUT)))
BINFILE   += $(addsuffix .bin, $(basename $(OUT)))
CLEANOBJS += $(OUT) $(OUT_SYMS) $(INITRD_OUT) $(OBJECTS) $(BINFILE) $(DISASM) .depend
CFLAGS    += $(HEADER_INC)
LDFLAGS   += $(LIB_INC)
ASFLAGS   += $(CFLAGS)

LD_SCRIPT_PROC = $(addsuffix .processed.ld, $(basename $(LD_SCRIPT)))

CLEANOBJS += $(LD_SCRIPT_PROC)

clean: clean-ak

clean-ak:
	-rm -f $(CLEANOBJS)

clean-libc:
	(cd uClibc; make clean)

format:
	find . -iname "*.[ch]" -not -path "./uClibc/*" -not -path "./userspace/dash/*" -exec clang-format -i {} \;

$(OUT): format libc $(OBJECTS) $(LD_SCRIPT_PROC)
	$(LD) -T $(LD_SCRIPT_PROC) -o $(OUT) $(LDFLAGS) $(OBJECTS) $(LIBS)

$(OUT_SYMS): $(OUT)
	$(OBJCOPY) --only-keep-debug $(OUT) $(OUT_SYMS)

$(BINFILE): $(OUT)
	$(OBJCOPY) -O binary $(OUT) $(BINFILE)

$(DISASM): $(OUT)
	$(OBJDUMP) -D $(OUT) > $(DISASM)

$(INITRD_OUT): $(USER_BINARIES) $(shell find $(INITRD_DIR) -type f)
	(cd $(INITRD_DIR) && ls | cpio -ov) > $(INITRD_OUT)

%.processed.ld: %.ld
	$(PREPROC) $(HEADER_INC) $^ -o $@

libc: uClibc/lib/libc.a

uClibc/lib/libc.a:
	(cd uClibc; make)

depend: .depend

.depend: $(SOURCES)
	-rm -f .depend
	$(CC) $(CFLAGS) -MM $^ >> $@

include .depend

.PHONY: all libc clean clean-ak clean-libc format
