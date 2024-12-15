#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <kernelapi.h>
#include <variables.h>
#include <driverbase.h>
#include <limine.h>

__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

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
    logPrintf("Initializing PIC\n");
    picRemap();
    virtSetOffset(hhdm_request.response->offset);
    logPrintf("Initializing PMM\n");
    initPMM();
    logPrintf("Initializing BackBuffer\n");
    uint64_t backbufferSize = gopGetPitch() * gopGetHeight();
    uint64_t backbufferPages = sizeToPages(backbufferSize);
    uint64_t startBackBuffer = allocZeroPagePhys();
    for(uint64_t i = 0;i < backbufferPages;i++) { // + 1 extra page
        allocZeroPagePhys();
    }   
    gopBackBuffer((uint32_t*)phys2Virt(startBackBuffer));
    logPrintf("Initializing Paging\n");
    initVMM();
    logPrintf("Initializing PIT\n");
    initPIT(100);
    osMain();
}