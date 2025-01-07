
#include <etc/acpi.h>
#include <uacpi/uacpi.h>
#include <uacpi/event.h>
#include <memory/pmm.h>
#include <fthelper.h>
#include <driverbase.h>
#include <etc/gfx.h>
#include <uacpi/tables.h>
#include <variables.h>
#include <etc/bmp.h>

char early = 1;

char isEarly() {
    return early;
}

void earlyAcpiInit() {
    early = 1;
    uacpi_status ret = uacpi_setup_early_table_access(pmmVBigAlloc(2048),2048 * PAGE_SIZE);
    if(ret != UACPI_STATUS_OK) {
        printf("UACPI Error: %d. Halting kernel\n",ret);
        cli();
        hlt();
    }
}

void acpiInit() {
    early = 0;
    uacpi_status ret = uacpi_initialize(0);
    
}

void bgrtDraw(uint64_t width,uint64_t height) {
    uacpi_table bgrt_pointer;
    uacpi_status ret = uacpi_table_find_by_signature("BGRT",&bgrt_pointer);
    if(ret != UACPI_STATUS_OK) {
        printf("BGRT Table doenst present by your firmware (%d)\n",ret);
    } else {
        bgrt_table* bgrt = (bgrt_table*)bgrt_pointer.virt_addr;
        bmp_info* bgrt_info = parseBMP((uint64_t)phys2Virt(bgrt->image_address));
        drawBMP((width / 2) - (bgrt_info->width / 2),(height / 2) - (bgrt_info->height / 2),(uint64_t)phys2Virt(bgrt->image_address));
    }
}