$(INITRD_DIR)/busybox:
	(cd userspace/busybox && \
	mkdir -p out; \
	make O=./out ak_defconfig; \
	cd out; \
	echo "CONFIG_EXTRA_CFLAGS=\"$(CFLAGS) -I /usr/include\"" >> .config; \
	echo "CONFIG_EXTRA_LDFLAGS=\"-Xlinker -melf_i386\"" >> .config; \
	echo "CONFIG_EXTRA_LDLIBS=\"-lc -lgcc\"" >> .config; \
	make)
	cp userspace/busybox/out/busybox $@

USER_BINARIES += $(INITRD_DIR)/busybox
