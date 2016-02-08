userspace/hello/hello: userspace/hello/main.c
	$(CC) $(LIB_INC) $(CFLAGS) -static -o $@ $^ /usr/lib/gcc/x86_64-redhat-linux/5.3.1/32/crt1.o /usr/lib/gcc/x86_64-redhat-linux/5.3.1/32/crti.o /usr/lib/gcc/x86_64-redhat-linux/5.3.1/32/crtn.o -lgcc

USER_BINARIES += userspace/hello/hello
