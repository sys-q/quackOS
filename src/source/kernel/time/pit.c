#include <kernelapi.h>
#include <driverbase.h>
#include <variables.h>
#include <stdint.h>


uint64_t ticks;
uint64_t pit_freq;

void pitTick() {
    ticks++;
}

void pitHandler(process_context_t* proc_ctx) {
    scheduling_lock(); // disable scheduling
    ticks++;
    if(ticks % (pit_freq / GOPFREQ) == 0) {
        gopSwap();
    }
    processWork(proc_ctx);
}

extern void pit_irq_32();

uint64_t pitCurrentTicks() {
    return ticks;
}

void initPIT(uint32_t freq) {
    uint16_t div = 1193182 / freq;
    pit_freq = freq;
    outb(0x43,0x36);
    outb(0x40,(uint8_t)(div & 0xFF));
    outb(0x40,(uint8_t)((div >> 8) & 0xFF));
    idtSetDescriptor(32,pit_irq_32,0x8E);
}