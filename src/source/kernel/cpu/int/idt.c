
#include <cpu/data.h>
#include <memory/heap.h>
#include <cpu/int/idt.h>

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

void idtSetDescriptor(uint8_t index,void* isr,uint8_t flags) {
    cpu_data_t* data = fetchData();
    idt_entry_t* idt = data->start_idt;
    uint8_t* vectors = data->vectors;
    idt[index].baselow = (uint64_t)isr & 0xFFFF;
    idt[index].cs = 0x08;
    idt[index].ist = 0;
    idt[index].attr = flags;
    idt[index].basemid = ((uint64_t)isr >> 16) & 0xFFFF;
    idt[index].basehigh = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    idt[index].reserved0 = 0;
    vectors[index] = 1;
}

void loadIDT(idtr_t idtr) {
    asm volatile("lidt %0" : : "m"(idtr));
}

extern void dummyIRQ();

uint8_t allocIRQ() {
    cpu_data_t* data = fetchData();
    uint8_t* vectors = data->vectors;
    for(uint8_t index = 32;index < 256;index++) {
        if(!vectors[index])
            return index;
    }
}

void freeIRQ(uint8_t index) {
    cpu_data_t* data = fetchData();
    uint8_t* vectors = data->vectors;
    vectors[index] = 0;
}

void idtInit() {
    cpu_data_t* data = fetchData();
    idtr_t* idtr = (idtr_t*)kmalloc(sizeof(idtr_t));
    idt_entry_t* idt = (idt_entry_t*)kmalloc(sizeof(idt_entry_t) * 256);
    uint8_t* vectors = (uint8_t*)kmalloc(256);
    data->start_idt = idt;
    data->vectors = vectors;
    idtr->base = (uint64_t)idt;
    idtr->limit = (uint16_t)sizeof(idt_entry_t) * 256 -1;

    for(uint8_t index = 0;index < 32;index++) {
        idtSetDescriptor(index,isr_table[index],0x8E);
        vectors[index] = 1;
    }
    for(uint8_t index = 32;index < 40;index++) {
        idtSetDescriptor(index,dummyIRQ,0x8E);
        vectors[index] = 0; // dummy 
    }
    vectors[0x80] = 1; // reserve int 0x80
    data->idtr = idtr;
    loadIDT(*idtr);
}