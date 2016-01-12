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
INITRD_DIR  := initrd/
CFLAGS		+= $(HEADER_INC)
LDFLAGS		+= $(LIB_INC)

SOURCES   :=
OBJECTS   :=
ASFLAGS   :=
DISASM    :=
BINFILE   :=
CLEANOBJS :=

all: $(OUT) $(DISASM) $(BINFILE)

include arch/makefile.mk
include kernel/makefile.mk

OBJECTS   += $(addsuffix .o, $(basename $(SOURCES)))
ASFLAGS   += $(CFLAGS)
DISASM    += $(addsuffix .dis, $(basename $(OUT)))
BINFILE   += $(addsuffix .bin, $(basename $(OUT)))
CLEANOBJS += $(OUT) $(INITRD_OUT) $(OBJECTS) $(BINFILE) $(DISASM) .depend

LD_SCRIPT_PROC = $(addsuffix .processed.ld, $(basename $(LD_SCRIPT)))

clean:
	-rm -f $(CLEANOBJS)

format:
	find . -iname "*.[ch]" -not -path "./uClibc/*" -exec clang-format -i {} \;

$(OUT): libc $(OBJECTS) $(LD_SCRIPT_PROC)
	$(LD) -T $(LD_SCRIPT_PROC) -o $(OUT) $(LDFLAGS) $(OBJECTS) $(LIBS)

$(OUT_SYMS): $(OUT)
	$(OBJCOPY) --only-keep-debug $(OUT) $(OUT_SYMS)

$(BINFILE): $(OUT)
	$(OBJCOPY) -O binary $(OUT) $(BINFILE)

$(DISASM): $(OUT)
	$(OBJDUMP) -D $(OUT) > $(DISASM)

$(INITRD_OUT): $(shell find $(INITRD_DIR) -type f)
	(cd $(INITRD_DIR) && ls | cpio -ov) > $(INITRD_OUT)

%.processed.ld: %.ld
	$(PREPROC) $(HEADER_INC) $^ -o $@

libc: uClibc/lib/libc.a

uClibc/lib/libc.a:
	make -e -C uClibc/

depend: .depend

.depend: $(SOURCES)
	-rm -f .depend
	$(CC) $(CFLAGS) -MM $^ -MF $@

include .depend

.PHONY: all libc clean format
