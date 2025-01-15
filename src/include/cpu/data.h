
#include <cpu/gdt.h>
#include <limine.h>
#include <cpu/int/idt.h>

#pragma once

typedef struct __attribute__((packed)) {
    struct limine_smp_info* smp_info;
    gdt_t* gdt;
    idt_entry_t* start_idt;
    uint8_t* vectors;
    gdt_pointer_t* gdtr;
    idtr_t* idtr;
} cpu_data_t;

cpu_data_t* createData();

cpu_data_t* fetchData();