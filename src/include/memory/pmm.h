
#include <stdint.h>

// PMM

typedef struct bitmap {
    uint64_t bits;
    uint64_t size;
    uint64_t page_count;
} bitmap_t;

typedef struct memory_entry {
    uint64_t base;
    uint64_t length;
    bitmap_t bitmap;
} memory_entry_t;

void pmmInit();

uint64_t pmmAlloc();

void pmmFree(uint64_t phys);

void* pmmVAlloc();

void pmmVFree(void* addr);

uint64_t pmmBigAlloc(uint64_t pages_count);

void pmmBigFree(uint64_t start,uint64_t sizeinpages);

void* pmmVBigAlloc(uint64_t pages_count);

void pmmVBigFree(void* start,uint64_t sizeinpages);

uint64_t pmmZero();

void* pmmVZero();

struct limine_memmap_request* pmmMap();