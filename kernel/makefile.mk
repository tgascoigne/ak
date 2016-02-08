SOURCES += $(addprefix kernel/, \
                kmain.c         \
				list.c          \
                module.c        \
                initrd.c)

include kernel/fs/makefile.mk
include kernel/io/makefile.mk
include kernel/proc/makefile.mk
include kernel/syscall/makefile.mk
include kernel/tty/makefile.mk
