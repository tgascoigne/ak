SOURCES += $(addprefix arch/, \
                hwinfo.c)

include arch/$(ARCH)/makefile.mk
