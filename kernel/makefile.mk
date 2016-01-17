SOURCES += $(addprefix kernel/, \
				list.c)

include kernel/fs/makefile.mk
include kernel/proc/makefile.mk
