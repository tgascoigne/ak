ARCH  ?= i386
BOARD ?= generic
CROSS_COMPILE :=

WARNFLAGS   := -Wall -Wextra -Wconversion -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unknown-pragmas

CFLAGS		:= -iwithprefix include -fms-extensions -fno-builtin -nostartfiles -static -g
LDFLAGS     := -static
LIB_INC     :=
HEADER_INC  := -I.
LIBS		:=
LD_SCRIPT   := arch/$(ARCH)/kernel.ld
OUT			:= kernel.elf
OUT_SYMS    := $(addsuffix .sym, $(basename $(OUT)))
INITRD_IMG  := initrd.img
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

CC		:= $(CROSS_COMPILE)gcc
LD		:= $(CROSS_COMPILE)ld
AS		:= $(CROSS_COMPILE)as
OBJDUMP := $(CROSS_COMPILE)objdump
OBJCOPY := $(CROSS_COMPILE)objcopy
PREPROC := $(CC) -E -x c -P

OBJECTS   += $(addsuffix .o, $(basename $(SOURCES)))
DISASM    += $(addsuffix .dis, $(basename $(OUT)))
BINFILE   += $(addsuffix .bin, $(basename $(OUT)))
CLEANOBJS += $(OUT) $(OUT_SYMS) $(INITRD_OUT) $(OBJECTS) $(BINFILE) $(DISASM) .depend
CFLAGS    += $(HEADER_INC) $(ARCH_CFLAGS)
LDFLAGS   += $(LIB_INC) $(ARCH_LDFLAGS)
ASFLAGS   += $(CFLAGS)

LD_SCRIPT_PROC = $(addsuffix .processed.ld, $(basename $(LD_SCRIPT)))

CLEANOBJS += $(LD_SCRIPT_PROC)

clean: clean-ak

clean-ak:
	-rm -f $(CLEANOBJS)

format:
	find . -iname "*.[ch]" -not -path "./uClibc/*" -not -path "./userspace/*" -exec clang-format -i {} \;

CRTI_OBJ=$(shell $(CC) $(CFLAGS) -print-file-name=crti.o)
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
CRTN_OBJ=$(shell $(CC) $(CFLAGS) -print-file-name=crtn.o)

%.o: CFLAGS += $(WARNFLAGS)
$(OUT): $(OBJECTS) $(LD_SCRIPT_PROC)
	$(CC) -T $(LD_SCRIPT_PROC) -o $(OUT) $(CFLAGS) $(addprefix -Xlinker ,$(LDFLAGS)) $(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJECTS) $(CRTEND_OBJ) $(CRTN_OBJ) $(LIBS)

$(OUT_SYMS): $(OUT)
	$(OBJCOPY) --only-keep-debug $(OUT) $(OUT_SYMS)

$(BINFILE): $(OUT)
	$(OBJCOPY) -O binary $(OUT) $(BINFILE)

$(DISASM): $(OUT)
	$(OBJDUMP) -D $(OUT) > $(DISASM)

%.processed.ld: %.ld
	$(PREPROC) $(HEADER_INC) $^ -o $@

depend: .depend

.depend: $(SOURCES)
	-rm -f .depend
	$(CC) $(CFLAGS) -MM $^ >> $@

include .depend

.PHONY: all clean clean-ak format
