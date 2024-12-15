#include <kernelapi.h>
#include <driverbase.h>
#include <stdint.h>

uint64_t ticks;
uint64_t pit_freq;

void pitHandler(uint16_t irq) {
    ticks++;
    if(ticks % 10 == 0)
        logPrintf("PIT: %d\n",ticks);
    if (ticks % (pit_freq / 60) == 0) { 
        gopSwap(); 
    }
}

extern void irq_32();

uint64_t pitCurrentTicks() {
    return ticks;
}

void initPIT(uint32_t freq) {
    uint16_t div = 1193182 / freq;
    pit_freq = freq;
    outb(0x43,0x36);
    outb(0x40,(uint8_t)(div & 0xFF));
    outb(0x40,(uint8_t)((div >> 8) & 0xFF));
    idtSetDescriptor(32,irq_32,0x8E);
    irqSetupHandler(32,pitHandler);
    picClearMask(0);
}