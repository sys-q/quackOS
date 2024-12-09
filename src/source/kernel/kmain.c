#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <kernelapi.h>
#include <limine.h>

void kmain(void) {
    initGop();
    textClearTextScreen();
    kernelInitTextMode();
    gopFastEnable();
    textSetBG(0x05);
    textClearTextScreen();
    logPrintf("Initializing GDT\n");
    initGDT();
    logPrintf("Initializing IDT\n");
    initIDT();
    osMain();
}