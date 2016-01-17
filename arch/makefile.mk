SOURCES += $(addprefix arch/, \
                hwinfo.c)

HEADER_INC += -I ./arch/$(ARCH)/

include arch/$(ARCH)/makefile.mk
