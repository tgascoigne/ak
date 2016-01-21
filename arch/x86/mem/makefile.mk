SOURCES += $(addprefix $(ARCH_PATH)/mem/, \
                brk.c                     \
                mmu.c                     \
                mmu_regs.c                \
                mmu_tlb.c                 \
                framealloc.c              \
                early_malloc.c)

# Wrap malloc and free to provide early_malloc until malloc is ready
LDFLAGS += --wrap=malloc --wrap=free
