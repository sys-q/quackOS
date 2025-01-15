
#include <time/hpet.h>
#include <cpu/int/apic.h>
#include <driverbase.h>
#include <stdint.h>
#include <time/lapic_timer.h>
#include <cpu/int/idt.h>
#include <cpu/data.h>

void test_irq() {
    printf("Received lapic timer 0x%p!\n",fetchData()->smp_info ? fetchData()->smp_info->processor_id : 0);
    sti();
    lapicEOI();
    while(1) {
        hlt();
    }
}

void lapicTimerEnable() {
    idtSetDescriptor(32,test_irq,0x8E);
    lapicWrite(0x3E0,0x3);
    lapicWrite(0x320,32 | (1 << 16));
    lapicWrite(0x380,0xFFFFFFFF);
    hpetSleep(10000);
    uint64_t ticks = 0xFFFFFFFF - lapicRead(0x390);
    lapicWrite(0x320,32 | (1 << 17));
    lapicWrite(0x3E0,0x3);
    lapicWrite(0x380,ticks);
    printf("LAPIC Ticks: 0x%p\n",ticks);
}