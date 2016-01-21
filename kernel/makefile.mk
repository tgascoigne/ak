SOURCES += $(addprefix kernel/, \
                kmain.c         \
				list.c          \
                module.c)

include kernel/fs/makefile.mk
include kernel/proc/makefile.mk
