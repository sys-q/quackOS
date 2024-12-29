
#include <stddef.h>
#include <kernelapi.h>
#include <variables.h>

void initHeapPage(size_t page) {
    kernel_heap_block_t* head = (kernel_heap_block_t*)pageToVirt(page);
    head->size = PAGE_SIZE;
    head->isfree = 1;
    head->next = (void*)0;
}

void initMultiHeapPage(size_t page,size_t page_count) {
    kernel_heap_block_t* head = (kernel_heap_block_t*)pageToVirt(page);
    head->size = PAGE_SIZE * page_count;
    head->isfree = 1;
    head->next = (void*)0;
}

void* pageMalloc(size_t page,size_t size) {
    kernel_heap_block_t* current = (kernel_heap_block_t*)pageToVirt(page);
    size_t totalsize = size + sizeof(kernel_heap_block_t);
    while(current) {
        if(current->isfree && current->size >= totalsize) {
            if(current->size > totalsize + sizeof(kernel_heap_block_t)) {
                kernel_heap_block_t* nextblock = (kernel_heap_block_t*)((uint8_t*)current + totalsize);
                nextblock->size = current->size - totalsize;
                nextblock->isfree = 1;
                nextblock->next = current->next;
                current->size = totalsize;
                current->next = nextblock;
            }
            current->isfree = 0;
            return (void*)((uint8_t*)current + sizeof(kernel_heap_block_t));
        }
        current=current->next;
    }
    return (void*)0;
}

void pageFree(void* ptr) {
    if(!ptr) {
        return;
    }
    kernel_heap_block_t* block = (kernel_heap_block_t*)((uint8_t*)ptr - sizeof(kernel_heap_block_t));
    block->isfree = 1;
}
