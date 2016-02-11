SOURCES += $(addprefix kernel/proc/, \
				task.c               \
                sched.c)

include kernel/proc/elf/makefile.mk
