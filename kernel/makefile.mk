SOURCES += $(addprefix kernel/, \
				task.c          \
				list.c)

include kernel/fs/makefile.mk
