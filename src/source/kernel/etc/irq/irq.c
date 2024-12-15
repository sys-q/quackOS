#include <kernelapi.h>
#include <variables.h>
#include <driverbase.h>


void (*vectors[0xFFFF - 1])(uint16_t) = {

};

void irqSetupHandler(uint16_t vector, void (*func)(uint16_t)) {
    vectors[vector] = func;
}

void irqHandler(uint16_t irq) {
   vectors[irq](irq);
   picEOI(irq);
}