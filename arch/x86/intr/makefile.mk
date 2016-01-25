SOURCES += $(addprefix $(ARCH_PATH)/intr/, \
				idt.c                      \
				isr_root.S                 \
				intr.c                     \
                pic.c)
