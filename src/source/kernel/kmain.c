#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <kernelapi.h>
#include <limine.h>

void kmain(void) {
    initGop();
    kernelInitTextMode();
    gopFastEnable();
    osMain();
}