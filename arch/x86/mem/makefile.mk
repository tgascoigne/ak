SOURCES += $(addprefix $(ARCH_PATH)/mem/, \
				brk.c 					  \
				mmu.c                     \
                framealloc.c              \
				early_malloc.c)

# Wrap malloc and free to provide early_malloc until malloc is ready
LDFLAGS += --wrap=malloc --wrap=free
