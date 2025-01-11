#include <stdint.h>
#include <cpu/gdt.h>
#include <memory/pmm.h>
#include <memory/heap.h>
#include <variables.h>
#include <driverbase.h>

gdt_t original_gdt = {
    {0,0,0,0,0,0}, // 0x0 null
    {0,0,0,0x9a,0xa2,0}, // 0x08 64 bit code
    {0,0,0,0x92,0xa0,0}, // 0x10 64 bit data
    {0,0,0,0xFA,0x20,0}, // 0x18 user code
    {0,0,0,0xF2,0,0}, // 0x20 user data
    {0x68,0,0,0x89,0x20,0,0,0} // 0x28 tss
};

void tssInit(gdt_t* gdt) {
    tss_t* tss = (tss_t*)kmalloc(sizeof(tss_t));
    tss->rsp[0] = (uint64_t)pmmVBigAlloc(TSS_STACK_SIZE_IN_PAGES);
    tss->ist[1] = 0;
    tss->iopb_offsset = sizeof(tss);
    gdt->tss.baselow16 = (uint64_t)tss & 0xFFFF;
    gdt->tss.basemid8 = ((uint64_t)tss >> 16) & 0xFF;
    gdt->tss.basehigh8 = ((uint64_t)tss >> 24) & 0xFF;
    gdt->tss.baseup32 = (uint64_t)tss >> 32;
}

void gdtInit() {
    gdt_pointer_t* gdt_pointer = (gdt_pointer_t*)kmalloc(sizeof(gdt_pointer_t));
    gdt_t* gdt = (gdt_t*)kmalloc(sizeof(gdt_t));
    memcpy(gdt,&original_gdt,sizeof(gdt_t));
    tssInit(gdt);
    gdt_pointer->size = sizeof(gdt_t) -1;
    gdt_pointer->base = (uint64_t)gdt;
    asmLoadGDT(gdt_pointer);
    asmLoadTSS();
}