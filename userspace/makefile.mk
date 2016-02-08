USER_BINARIES :=

include userspace/hello/makefile.mk

userspace: $(USER_BINARIES)
	cp $< $(INITRD_DIR)
