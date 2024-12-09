#include <stddef.h>
#include <driverbase.h>
#include <kernelapi.h>
#include <limine.h>



bitmap_t bitmapInit(uint64_t base, size_t size) {
    bitmap_t bmp;
    bmp.size = size;
    bmp.bits = base; // (uint64_t *)malloc(((size + 63) / 64) * sizeof(uint64_t))
    return bmp;
}

void bitmapSetBit(bitmap_t *bmp, size_t index) {
    if (index >= bmp->size) return;
    phys2Virt(bmp->bits)[index / 8] |= (1ULL << (index % 8));
}

void bitmapClearBit(bitmap_t *bmp, size_t index) {
    if (index >= bmp->size) return; 
    phys2Virt(bmp->bits)[index / 8] &= ~(1ULL << (index % 8));
}

int bitmapIsBitSet(bitmap_t *bmp, size_t index) {
    if (index >= bmp->size) return 0; 
    return (phys2Virt(bmp->bits)[index / 8] & (1ULL << (index % 8))) != 0;
}

memory_entry_t biggest_entry;

memory_entry_t* getBiggestEntry() {
    return &biggest_entry;
}

uint64_t* earlyMalloc(size_t size) {
    uint64_t* base = (uint64_t*)((uint64_t)biggest_entry.base + biggest_entry.skip);
    biggest_entry.skip += size;
    return base;
}

void initPMM() {

}