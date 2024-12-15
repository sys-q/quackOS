#include <kernelapi.h>
#include <driverbase.h>
#include <stdint.h>

uint64_t ticks;

void pitHandler(uint16_t irq) {
    ticks++;
    logPrintf("PIT: %d\n",ticks);
}

extern void irq_32();

uint64_t pitCurrentTicks() {
    return ticks;
}

void initPIT(uint32_t freq) {
    uint16_t div = PIT_FREQUENCY / freq;
    outb(PIT_COMMAND,0x36);
    outb(PIT_CHANNEL0,(uint8_t)(div & 0xFF));
    outb(PIT_CHANNEL0,(uint8_t)((div >> 8) & 0xFF));
    idtSetDescriptor(32,irq_32,0x8E);
    irqSetupHandler(32,pitHandler);
    picClearMask(0);
}