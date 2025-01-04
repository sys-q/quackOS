
#include <etc/acpi.h>
#include <uacpi/uacpi.h>
#include <uacpi/event.h>
#include <fthelper.h>
#include <driverbase.h>

char early = 1;

char isEarly() {
    return early;
}

void earlyAcpiInit() {
    early = 1;
    uacpi_status ret = uacpi_initialize(0);
    if(ret != UACPI_STATUS_OK) {
        printf("UACPI Error: %d. Halting kernel\n",ret);
        cli();
        hlt();
    } else {
        printf("UACPI is good !!!\n");
    }
}

void acpiInit() {
    earlyAcpiInit();
    early = 0;
    
}