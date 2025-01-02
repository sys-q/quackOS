#include <stdint.h>
#include <variables.h>
#include <limine.h>
#include <driverbase.h>
#include <fthelper.h>
#include <memory/pmm.h>

__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

void bitmapSetBit(bitmap_t *bmp, uint64_t index) {
    if (index >= bmp->size) return;
    phys2Virt(bmp->bits)[index / 8] |= (1ULL << (index % 8));
}

void bitmapClearBit(bitmap_t *bmp, uint64_t index) { 
    if (index >= bmp->size) return; 
    phys2Virt(bmp->bits)[index / 8] &= ~(1ULL << (index % 8));
}

int bitmapIsBitSet(bitmap_t *bmp, uint64_t index) {
    if (index >= bmp->size) return 0; 
    return (phys2Virt(bmp->bits)[index / 8] & (1ULL << (index % 8))) != 0;
}

memory_entry_t biggest_entry;

void pmmInit() {
    uint64_t top = 0;
    struct limine_memmap_entry* current_entry;
    for(uint64_t i = 0; i < memmap_request.response->entry_count;i++) {
        current_entry = memmap_request.response->entries[i];
        if(current_entry->type == LIMINE_MEMMAP_USABLE) {
            if(current_entry->length > top) {
                top = current_entry->length;
                biggest_entry.base = current_entry->base;
                biggest_entry.length = current_entry->length;
            }
        }
    }

    // init bitmap

    uint64_t pageCount = biggest_entry.length / PAGE_SIZE;
    uint64_t bitmapSize = (pageCount + 7) / 8;
    uint64_t bitmapPages = bitmapSize / PAGE_SIZE;
    biggest_entry.bitmap.size = bitmapSize;
    biggest_entry.bitmap.bits = biggest_entry.base;
    biggest_entry.bitmap.page_count = pageCount;

    memset(phys2Virt(biggest_entry.base),0,bitmapSize);

    for(uint64_t i = 0;i < bitmapPages;i++) {
        bitmapSetBit(&biggest_entry.bitmap,i);
    }

    

}

// return phys
uint64_t pmmAlloc() {
    for(uint64_t i = 0;i < biggest_entry.bitmap.page_count;i++) {
        if(!bitmapIsBitSet(&biggest_entry.bitmap,i)) {
            bitmapSetBit(&biggest_entry.bitmap,i);
            return (i * PAGE_SIZE) + biggest_entry.base;
        }
    }
    
}

void pmmFree(uint64_t phys) {
    uint64_t page = (phys - biggest_entry.base) / PAGE_SIZE;
    bitmapClearBit(&biggest_entry.bitmap,page);
}

void* pmmVAlloc() {
    return phys2Virt(pmmAlloc());
}

void pmmVFree(void* addr) {
    pmmFree((uint64_t)virt2Phys((uint64_t)addr));
}

uint64_t pmmBigAlloc(uint64_t pages_count) {
    uint64_t goodCount = 0;
    uint64_t goodStart = 0;
    for(uint64_t i = 0; i < biggest_entry.bitmap.page_count;i++) {
        if(!bitmapIsBitSet(&biggest_entry.bitmap,i)) {
            goodCount++;
            if(!goodStart)
                goodStart = i;

            if(pages_count == goodCount) { 
                for(uint64_t i = goodStart;i <= goodCount;i++) {
                    bitmapSetBit(&biggest_entry.bitmap,i);
                }
                return (goodStart * PAGE_SIZE) + biggest_entry.base;
            }
        } else {
            goodCount = 0;
            goodStart = 0;
        }
    }
    return 0;
}

void pmmBigFree(uint64_t start,uint64_t sizeinpages) {
    for(uint64_t i = (start - biggest_entry.base) / PAGE_SIZE; i < sizeinpages;i++) {
        bitmapClearBit(&biggest_entry.bitmap,i);
    }
}

void* pmmVBigAlloc(uint64_t pages_count) {
    uint64_t phys = pmmBigAlloc(pages_count);
    if(!phys)
        return 0;
    return (void*)phys2Virt(phys);
}

void pmmVBigFree(void* start,uint64_t sizeinpages) {
    uint64_t phys_start = (uint64_t)virt2Phys((uint64_t)start);
    if(!phys_start)
        return;
    pmmBigFree(phys_start,sizeinpages);
}