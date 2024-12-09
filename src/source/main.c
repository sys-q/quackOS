#include <variables.h>
#include <kernelapi.h>
#include <driverbase.h>

void osMain() {
    logPrintf("Test\n");
    uint64_t val = 0x1F4;
    __asm__ ("movq %0, %%r15" : : "r" (val));
    asm volatile("int $0x90");
    cli();
    while(true) {hlt();}
}