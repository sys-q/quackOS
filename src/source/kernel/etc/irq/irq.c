#include <kernelapi.h>
#include <variables.h>
#include <driverbase.h>


void (*vectorss[0xFFFF - 1])(uint16_t) = {

};

void irqSetupHandler(uint16_t vector, void (*func)(uint16_t)) {
    vectorss[vector] = func;
}

void irqHandler(uint16_t irq) {
   vectorss[irq](irq);
   picEOI(irq);
}