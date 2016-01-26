SOURCES += $(addprefix $(ARCH_PATH)/intr/, \
				idt.c                      \
				isr_root.S                 \
				intr.c                     \
                pit.c                      \
                pic.c)
