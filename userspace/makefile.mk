USER_BINARIES :=

USERSPACE_LIBS := -lc -lgcc

include userspace/hello/makefile.mk

CLEANOBJS += $(USER_BINARIES)
