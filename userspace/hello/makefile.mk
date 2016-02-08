initrd/hello: userspace/hello/main.c
	$(CC) $(CFLAGS) -static -o $@ $<

USER_BINARIES += initrd/hello
