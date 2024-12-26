#include <kernelapi.h>

extern void isr_0();
extern void isr_1();
extern void isr_2();
extern void isr_3();
extern void isr_4();
extern void isr_5();
extern void isr_6();
extern void isr_7();
extern void isr_8();
extern void isr_9();
extern void isr_10();
extern void isr_11();
extern void isr_12();
extern void isr_13();
extern void isr_14();
extern void isr_15();
extern void isr_16();
extern void isr_17();
extern void isr_18();
extern void isr_19();
extern void isr_20();
extern void isr_30();

void isr_reserved() {
    // nothing
}

void* isr_table[32] = {
    isr_0,isr_1,isr_2,isr_3,isr_4,isr_5,isr_6,isr_7,isr_8,isr_9,isr_10,isr_11,isr_12,isr_13,isr_14,isr_reserved,isr_16,isr_17,
    isr_18,isr_19,isr_20,isr_reserved,isr_reserved,isr_reserved,isr_reserved,isr_reserved,isr_reserved,isr_reserved,isr_reserved,isr_reserved,isr_30
};

static idt_entry_t idt[256];
static idtr_t idtr;
static int vectors[256];

void idtSetDescriptor(uint8_t index,void* isr,uint8_t flags) {
    idt[index].baselow = (uint64_t)isr & 0xFFFF;
    idt[index].cs = 0x08;
    idt[index].ist = 0;
    idt[index].attr = flags;
    idt[index].basemid = ((uint64_t)isr >> 16) & 0xFFFF;
    idt[index].basehigh = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    idt[index].reserved0 = 0;
}

void loadIDT() {
    asm volatile("lidt %0" : : "m"(idtr));
    asm volatile("sti");
}

extern void dummyIRQ();

void initIDT() {
    idtr.base = (uint64_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * 256 -1;
    for(uint8_t index = 0;index < 32;index++) {
        idtSetDescriptor(index,isr_table[index],0x8E);
        vectors[index] = 1;
    }
    for(uint8_t index = 32;index < 40;index++) {
        idtSetDescriptor(index,dummyIRQ,0x8E);
        vectors[index] = 1;
    }
    loadIDT();
}