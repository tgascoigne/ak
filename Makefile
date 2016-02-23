ARCH  ?= x86
BOARD ?= generic

CC		:= $(CROSS_COMPILE)clang
LD		:= $(CROSS_COMPILE)ld
AS		:= $(CROSS_COMPILE)as
OBJDUMP := $(CROSS_COMPILE)objdump
OBJCOPY := $(CROSS_COMPILE)objcopy
PREPROC := $(CC) -E -x c -P

WARNFLAGS   := -Wall -Wextra -Wpedantic -Werror -Wconversion -Wshadow -Wno-unused-parameter -Wno-microsoft -Wno-unused-function

CFLAGS		:= -iwithprefix include -fms-extensions -g
LDFLAGS     := -static
LIB_INC     :=
HEADER_INC  := -I.
LIBS		:= -lc -lgcc
LD_SCRIPT   := arch/$(ARCH)/kernel.ld
OUT			:= kernel.elf
OUT_SYMS    := $(addsuffix .sym, $(basename $(OUT)))
INITRD_OUT  := initrd.img
INITRD_DIR  := initrd

LIBC_PREFIX := $(shell pwd)/out
LIBC_TARGET :=

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

LIBC_OUT     := $(LIBC_PREFIX)/usr/$(LIBC_TARGET)/
LIBC_HEADERS := $(LIBC_OUT)/usr/include/
LIBC_LIBDIR  := $(LIBC_OUT)/usr/lib/
HEADER_INC   += -isystem $(LIBC_HEADERS)
LIB_INC      += -L$(LIBC_LIBDIR)
DYN_LIB_INC  += $(LIBC_OUT)/usr/lib/:$(LIBC_OUT)/lib/

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
	make -C uClibc clean
	rm -rf $(LIBC_PREFIX)

format:
	find . -iname "*.[ch]" -not -path "./uClibc/*" -not -path "./userspace/*" -exec clang-format -i {} \;

%.o: CFLAGS += $(WARNFLAGS)
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

libc: $(LIBC_OUT)

$(LIBC_OUT):
	PREFIX=$(LIBC_PREFIX) make -C uClibc install

depend: .depend

.depend: $(SOURCES)
	-rm -f .depend
	$(CC) $(CFLAGS) -MM $^ >> $@

include .depend

.PHONY: all libc clean clean-ak clean-libc format
