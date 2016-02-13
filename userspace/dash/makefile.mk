$(INITRD_DIR)/dash:
	(cd userspace/dash && \
	CFLAGS="-g -DHAVE_KILLPG -DHAVE_STRTOD -march=i686 -m32 -nostdlib -I /usr/i386-linux-uclibc/usr/include/ -L $$HOME/ak/uClibc/lib/" LIBS="$$HOME/ak/uClibc/lib/crt1.o -lm -lc -lgcc" ./configure --host=x86_64-redhat-linux --target=i386-linux-uclibc --enable-static && \
	make)
	cp userspace/dash/src/dash $@

USER_BINARIES += $(INITRD_DIR)/dash
