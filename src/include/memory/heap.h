
#include <stdint.h>

typedef struct kernel_heap_block {
    uint64_t size;
    uint8_t isfree;
    struct kernel_heap_block* next;
} kernel_heap_block_t;

void initHeap(uint64_t start,uint64_t page_count);

void* heapMalloc(uint64_t start,uint64_t size);

void heapFree(void* ptr);

void kmallocInit(uint64_t start,uint64_t size_in_pages);

void* kmalloc(uint64_t size);

void kfree(void* ptr);