
#include <etc/acpi.h>
#include <uacpi/uacpi.h>
#include <uacpi/event.h>


char early = 1;

char isEarly() {
    return early;
}

void earlyAcpiInit() {
    early = 1;
    uacpi_initialize(0);
}

void acpiInit() {
    early = 0;
    uacpi_status ret = uacpi_initialize(0);
}