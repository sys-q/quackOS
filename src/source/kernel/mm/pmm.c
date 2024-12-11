#include <stddef.h>
#include <driverbase.h>
#include <kernelapi.h>
#include <variables.h>
#include <limine.h>

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

struct limine_memmap_request* getMemMap() {
    return &memmap_request;
}

memory_entry_t* getBiggestEntry() {
    return &biggest_entry;
}

uint64_t* earlyMalloc(uint64_t size) {
    uint64_t* base = (uint64_t*)((uint64_t)biggest_entry.base + biggest_entry.skip);
    biggest_entry.skip += size;
    return base;
}

bitmap_t bitmapInit(uint64_t size) {
    bitmap_t bmp;
    bmp.size = size;
    bmp.bits = (uint64_t)earlyMalloc(((size + 63) / 64) * sizeof(uint64_t));
    return bmp;
}

void initPMM() {
    struct limine_memmap_entry* current_entry;
    uint64_t top = 0;
    for(uint64_t i = 0; i < memmap_request.response->entry_count;i++) {
        current_entry = memmap_request.response->entries[i];
        if(current_entry->length > top && current_entry->type == LIMINE_MEMMAP_USABLE) {
            top = current_entry->length;
            biggest_entry.base = current_entry->base;
            biggest_entry.length = current_entry->length;
        }
    }
    uint64_t pageCount = biggest_entry.length / PAGE_SIZE;
    uint64_t bitmapSize = (pageCount + 7) / 8;
    uint64_t bitmapPages = bitmapSize / PAGE_SIZE;
    biggest_entry.bitmap = bitmapInit(bitmapSize);
    biggest_entry.bitmap.pages_count = pageCount;
    biggest_entry.bitmap.next = bitmapPages + 1;
    for(uint64_t i = 0;i < bitmapPages;i++) {
        bitmapSetBit(&biggest_entry.bitmap,i);
    }
       

}

uint64_t allocPage() {
    cli();
    uint64_t next_page = biggest_entry.bitmap.next;
    if(next_page != 0) {
        if(!bitmapIsBitSet(&biggest_entry.bitmap,next_page))
            bitmapSetBit(&biggest_entry.bitmap,next_page);
        if(!bitmapIsBitSet(&biggest_entry.bitmap,next_page + 1)){
            biggest_entry.bitmap.next++;
            biggest_entry.bitmap.pages_count++;
            sti();
            return next_page;
        }
        else 
            biggest_entry.bitmap.next = 0;
    } 
    
    for(uint64_t i =0; i < biggest_entry.bitmap.pages_count;i++) {
        if(!bitmapIsBitSet(&biggest_entry.bitmap,i)) {
            if(biggest_entry.bitmap.pages_count == i)
                biggest_entry.bitmap.next = 0;
            bitmapSetBit(&biggest_entry.bitmap,i);
            biggest_entry.bitmap.pages_count++;
            sti();
            return i;
        }
    }

    return 0;
}

void freePage(uint64_t page) {
    bitmapClearBit(&biggest_entry.bitmap,page);
    biggest_entry.bitmap.pages_count--;
}

uint64_t allocZeroPage() {
    uint64_t page = allocPage();
    memset(phys2Virt(biggest_entry.base + (page*PAGE_SIZE)),0,PAGE_SIZE);
    return page;
}

uint64_t allocZeroPagePhys() {
    uint64_t page = allocZeroPage();
    return biggest_entry.base + (page*PAGE_SIZE);
}

uint64_t allocPages(uint64_t num_pages) {
    if (num_pages == 0) {
        return 0; 
    }
    cli();
    uint64_t start_index = biggest_entry.bitmap.next;
    uint64_t contiguous_count = 0;
    for (uint64_t i = start_index; i < biggest_entry.bitmap.pages_count; i++) {
        if (!bitmapIsBitSet(&biggest_entry.bitmap, i)) {
            contiguous_count++;
            if (contiguous_count == num_pages) {
                for (uint64_t j = 0; j < num_pages; j++) {
                    bitmapSetBit(&biggest_entry.bitmap, i - num_pages + 1 + j);
                }
                biggest_entry.bitmap.next = (i + 1) % biggest_entry.bitmap.pages_count;
                biggest_entry.bitmap.pages_count += num_pages; 
                sti();
                return i - num_pages + 1; 
            }
        } else {
            contiguous_count = 0;
        }
    }
    for (uint64_t i = 0; i < start_index; i++) {
        if (!bitmapIsBitSet(&biggest_entry.bitmap, i)) {
            contiguous_count++;
            if (contiguous_count == num_pages) {
                for (uint64_t j = 0; j < num_pages; j++) {
                    bitmapSetBit(&biggest_entry.bitmap, i - num_pages + 1 + j);
                }
                biggest_entry.bitmap.next = (i + 1) % biggest_entry.bitmap.pages_count;
                biggest_entry.bitmap.pages_count += num_pages; 
                sti();
                return i - num_pages + 1; 
            }
        } else {
            contiguous_count = 0; 
        }
    }
    return 0; 
}

void freePages(uint64_t start_page, uint64_t num_pages) {
    cli();
    for (uint64_t i = 0; i < num_pages; i++) {
        bitmapClearBit(&biggest_entry.bitmap, start_page + i); 
    }
    biggest_entry.bitmap.pages_count -= num_pages; 
    if (biggest_entry.bitmap.next != 0 && start_page < biggest_entry.bitmap.next) {
        biggest_entry.bitmap.next = start_page;
    }
    sti();
}

uint64_t sizeToPages(uint64_t size) {
    uint64_t pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    if(size > PAGE_SIZE - 10)
        pages++;
    return pages > 0 ? pages : 1;
}
