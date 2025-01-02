
#include <stdint.h>

#define ROUNDUP(VALUE,ROUND) ((VALUE + (ROUND - 1)) / ROUND)
#define ALIGNPAGEUP(VALUE) (ROUNDUP(VALUE,PAGE_SIZE) * PAGE_SIZE)
#define ALIGNPAGEDOWN(VALUE) ((VALUE / PAGE_SIZE) * PAGE_SIZE)
#define PTE_MASK_VALUE 0x000ffffffffff000
#define PTE_MASK(VALUE,FLAGS) (VALUE | FLAGS)
#define PTE_UNMASK(VALUE) (VALUE & PTE_MASK_VALUE)
#define PTE_PRESENT (1 << 0)
#define PTE_WRITABLE (1 << 1)
#define PTE_USER (1 << 2)
#define PTE_CACHE_MMIO (1 << 3)
#define PML4 0x01
#define PDP 0x02
#define PD 0x03
#define PT 0x04

uint64_t* pagingLevel(uint64_t* table,uint64_t entry,uint64_t flags);

void pagingMap(uint64_t* pml4,uint64_t phys,uint64_t virt,uint64_t flags);

void pagingMapEntry(uint64_t* pml4,uint16_t type);

void pagingMapEntryFlag(uint64_t* pml4,uint16_t type,uint64_t flag);

void pagingKernel(uint64_t* pml4);

void pagingPAT(uint8_t idx, uint8_t type);

void pagingActivateKernel();

void pagingInit();