SOURCES += $(addprefix kernel/syscall/, \
                syscall.c               \
                fdio.c                  \
                proc.c                  \
                mem.c                   \
                uname.c                 \
                signal.c                \
                user.c                  \
                thread.c)
