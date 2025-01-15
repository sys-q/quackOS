
#include <stdint.h>

// IDT

#pragma once

typedef struct __attribute__((packed)) {
    uint64_t vector;
    uint64_t error_code;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} error_frame_t ;

typedef struct __attribute__((packed)) {
    uint16_t baselow;
    uint16_t cs;
    uint8_t ist;
    uint8_t attr;
    uint16_t basemid;
    uint32_t basehigh;
    uint32_t reserved0;
} idt_entry_t;

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint64_t base;
} idtr_t;

void idtSetDescriptor(uint8_t index,void* isr,uint8_t flags);

uint8_t allocIRQ();

void freeIRQ(uint8_t index);

void idtInit();