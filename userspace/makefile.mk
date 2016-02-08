USER_BINARIES :=

include userspace/hello/makefile.mk

CLEANOBJS += $(USER_BINARIES)
