#include <stdint.h>
#include <kernelapi.h>

uint8_t tssStack[65536];

gdt_t gdt = {
    {0,0,0,0,0,0}, // 0x0 null
    {0,0,0,0x9a,0xa2,0}, // 0x08 64 bit code
    {0,0,0,0x92,0xa0,0}, // 0x10 64 bit data
    {0,0,0,0xFA,0x20,0}, // 0x18 user code
    {0,0,0,0xF2,0,0}, // 0x20 user data
    {0x68,0,0,0x89,0x20,0,0,0} // 0x28 tss
};

gdt_pointer_t gdt_pointer;
tss_t tss;

void InitTSS() {
    tss.rsp[0] = (uint64_t)tssStack;
    tss.ist[1] = 0;
    tss.iopb_offsset = sizeof(tss);
    gdt.tss.baselow16 = (uint64_t)&tss & 0xFFFF;
    gdt.tss.basemid8 = ((uint64_t)&tss >> 16) & 0xFF;
    gdt.tss.basehigh8 = ((uint64_t)&tss >> 24) & 0xFF;
    gdt.tss.baseup32 = (uint64_t)&tss >> 32;
}

void initGDT() {
    InitTSS();
    gdt_pointer.size = sizeof(gdt_t) -1;
    gdt_pointer.base = (uint64_t)&gdt;
    asmLoadGDT(&gdt_pointer);
    asmLoadTSS();
}