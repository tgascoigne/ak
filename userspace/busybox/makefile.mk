$(INITRD_DIR)/busybox:
	(cd userspace/busybox && \
	mkdir -p out; \
	make O=./out ak_defconfig; \
	cd out; \
	LDFLAGS="-Xlinker -melf_i386" \
	CFLAGS="-g -DHAVE_KILLPG -DHAVE_STRTOD -march=i686 -m32 -nostdlib -I /usr/i386-linux-uclibc/usr/include/ -L $$HOME/ak/uClibc/lib/" LIBS="$$HOME/ak/uClibc/lib/crt1.o -lm -lc -lgcc" \
	make)
	cp userspace/busybox/out/busybox $@

USER_BINARIES += $(INITRD_DIR)/busybox
