SOURCES += $(addprefix $(ARCH_PATH)/mem/, \
                brk.c                     \
                gdt.c                     \
                gdt_load.S                \
                mmu.c                     \
                mmu_clone_dir.c           \
                framealloc.c              \
                early_malloc.c)

# Wrap malloc and free to provide early_malloc until malloc is ready
LDFLAGS += --wrap=malloc --wrap=free
