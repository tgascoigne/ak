USER_BINARIES :=

CRTDIR := ./uClibc/lib
USERSPACE_LIBS := $(CRTDIR)/crt1.o -lc -lgcc

include userspace/hello/makefile.mk

CLEANOBJS += $(USER_BINARIES)
