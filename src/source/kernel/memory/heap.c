
#include <stdint.h>
#include <memory/heap.h>
#include <variables.h>

void initHeap(uint64_t start,uint64_t page_count) {
    kernel_heap_block_t* head = (kernel_heap_block_t*)start;
    head->size = PAGE_SIZE * page_count;
    head->isfree = 1;
    head->next = (void*)0;
}

void* heapMalloc(uint64_t start,uint64_t size) {
    kernel_heap_block_t* current = (kernel_heap_block_t*)start;
    uint64_t totalsize = size + sizeof(kernel_heap_block_t);
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

void heapFree(void* ptr) {
    if(!ptr) {
        return;
    }
    kernel_heap_block_t* block = (kernel_heap_block_t*)((uint8_t*)ptr - sizeof(kernel_heap_block_t));
    block->isfree = 1;
}

uint64_t kmalloc_start;

void kmallocInit(uint64_t start,uint64_t size_in_pages) {
    kmalloc_start = start;
    initHeap(start,size_in_pages);
}

void* kmalloc(uint64_t size) {
    return heapMalloc(kmalloc_start,size);
}

void kfree(void* ptr) {
    heapFree(ptr);
}