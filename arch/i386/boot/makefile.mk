SOURCES += $(addprefix $(ARCH_PATH)/boot/, \
				multiboot.S				   \
				boot.S	     			   \
				init.c                     \
				multibootimpl.c)
