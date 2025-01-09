
// nah fuck pit

#include <stdint.h>
#include <cpu/int/idt.h>
#include <time/pit.h>
#include <memory/paging.h>
#include <cpu/int/pic.h>
#include <uacpi/acpi.h>
#include <uacpi/uacpi.h>
#include <uacpi/status.h>
#include <uacpi/tables.h>
#include <driverbase.h>
#include <fthelper.h>

uint32_t pit_freq;
extern void irq_32();

void pitInit(uint32_t freq) {
    uint16_t div = 1193182 / freq;
    pit_freq = freq;
    outb(0x43,0x36);
    outb(0x40,(uint8_t)(div & 0xFF));
    outb(0x40,(uint8_t)((div >> 8) & 0xFF));
    idtSetDescriptor(32,irq_32,0x8E);
}

