SOURCES += $(addprefix $(ARCH_PATH)/intr/, \
				idt.c                      \
				isr_root.S                 \
                pit.c                      \
                pic.c)
