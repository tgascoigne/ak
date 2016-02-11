THISDIR=$(dir $(lastword $(MAKEFILE_LIST)))

$(INITRD_DIR)/hello: $(THISDIR)/main.o
	$(LD) -o $@ $(LDFLAGS) $< $(USERSPACE_LIBS)

USER_BINARIES += $(INITRD_DIR)/hello
