
#include <fthelper.h>
#include <stdint.h>
#include <uacpi/acpi.h>
#include <uacpi/tables.h>
#include <uacpi/status.h>
#include <uacpi/utilities.h>
#include <uacpi/resources.h>
#include <uacpi/types.h>
#include <uacpi/event.h>
#include <cpu/int/apic.h>
#include <cpu/int/pic.h>

void apicInit() {
    uacpi_table apic;
    uacpi_status ret = uacpi_table_find_by_signature("APIC",&apic);

    if(ret == UACPI_STATUS_OK) { 
        struct acpi_madt* apic_struct = ((struct acpi_madt*) apic.virt_addr);
        printf("APIC: 0x%p 0x%p 0x%p\n",apic_struct->entries[0].length,apic_struct,apic_struct->flags);
    }

}