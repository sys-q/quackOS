
#include <etc/acpi.h>
#include <uacpi/uacpi.h>
#include <uacpi/event.h>
#include <memory/pmm.h>
#include <fthelper.h>
#include <driverbase.h>
#include <variables.h>

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