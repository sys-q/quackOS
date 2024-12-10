#include <variables.h>
#include <kernelapi.h>
#include <driverbase.h>

void osMain() {
    uint64_t testalloc = allocPage();
    uint64_t testalloc2 = allocPage();
    freePage(testalloc2);
    uint64_t testalloc3 = allocPage();
    logPrintf("Bitmap: 0x%p, Size: 0x%p, MaxPages: 0x%p. PageAlloc: 0x%p, PageAlloc2: 0x%p, PageAlloc2 bitmap state after free: %d PageAlloc3: 0x%p\n",getBiggestEntry()->bitmap.bits,getBiggestEntry()->bitmap.size,getBiggestEntry()->bitmap.pages_count,testalloc,testalloc2,bitmapIsBitSet(&getBiggestEntry()->bitmap,testalloc2),testalloc3);
    cli();
    while(true) {hlt();}
}