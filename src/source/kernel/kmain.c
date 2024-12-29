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

void test() {
    while (1)
    {
        printf("1");
        hlt();

    }
    
}

void kmain_task();

void kmain(void) {
    initGop();
    textClearTextScreen();
    kernelInitTextMode();
    gopFastDisable();
    textSetBG(0x05);
    textClearTextScreen();
    logPrintf("Initializing GDT\n");
    initGDT();
    logPrintf("Initializing IDT\n");
    initIDT();
    disasmInit();
    logPrintf("Initializing PIC\n");
    picRemap();
    virtSetOffset(hhdm_request.response->offset);
    logPrintf("Initializing PMM\n");
    initPMM();
    logPrintf("Initializing Paging\n");
    initVMM();
    logPrintf("Initializing BackBuffer\n");
    uint64_t back_size = gopGetHeight() * gopGetPitch();
    uint64_t back_pages = sizeToPages(back_size) + 1;
    uint64_t base = allocPages(back_pages);
    uint64_t base_phys = (base * PAGE_SIZE) + getBiggestEntry()->base;
    vmmActivatePML(virt2Phys((uint64_t)vmmGetPMM()));
    vmmSetBackbuffer(base_phys);
    vmmMapBackBuffer(vmmGetKernel());
    vmmActivatePML(virt2Phys((uint64_t)vmmGetKernel()));
    gopBackBuffer(phys2Virt(base_phys));
    textClearTextScreen();
    logPrintf("PMM, Paging, GDT, IDT and BackBuffer initializied successfuly !\n");
    logPrintf("Initializing Scheduling\n");
    processQueue(1,0); // head
        
    process_t* kernel_init_task = processQueue((uint64_t)kmain_task,0);
    kernel_init_task->context.cr3 = (uint64_t)virt2Phys((uint64_t)vmmGetKernel());
    for(uint64_t i = ALIGNPAGEDOWN((uint64_t)virt2Phys(kernel_init_task->context.rsp));i < (256 * PAGE_SIZE) + ALIGNPAGEUP((uint64_t)virt2Phys(kernel_init_task->context.rsp));i += PAGE_SIZE) {
        vmmMapPage(vmmGetKernel(),i,(uint64_t)phys2Virt(i),PTE_PRESENT | PTE_WRITABLE);
    }
    vmmMapPage(vmmGetKernel(),(uint64_t)virt2Phys((uint64_t)kernel_init_task),(uint64_t)kernel_init_task,PTE_PRESENT | PTE_WRITABLE);
    processQueue((uint64_t)test,0);
    gopSwap();
    logPrintf("Initializing PIT\n");
    initPIT(1000);

    scheduling_unlock();    
    while(1) {
        hlt();
    }
}


void kmain_task() {
    scheduling_lock();
    logPrintf("Initializing ACPI\n");
    gopSwap();
    initACPI();
    
    textSetFG(0xFFFF00);
    printf("\n\n                __\n");
    printf("            ___( o)>\n");
    printf("             \\ <_. )\n");
    printf("              `---' \n\n");
    printf("        Welcome to quackOS !\n\n\n");
    textSetFG(0xFFFFFF);
    scheduling_unlock();
    while(1) {
        hlt();
    }
}