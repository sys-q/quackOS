
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
#include <memory/paging.h>
#include <cpu/int/pic.h>
#include <scheduling/timers.h>
#include <driverbase.h>

uint64_t lapicVBase;

void lapicWrite(uint32_t reg,uint32_t value) {
    *(uint32_t*)(lapicVBase + reg) = value;
}

uint32_t lapicRead(uint32_t reg) {
    return *(uint32_t*)(lapicVBase + reg);
}

void lapicEnable(uint64_t phys) {
    wrmsr64(0x1b,(phys & 0xfffff0000) | 0x800);
}

uint32_t lapicID() {
    return lapicRead(0x20) >> 24;
}

void lapicTimerCalibrate() {
    lapicWrite(0,0);
}

void lapicEOI() {
    lapicWrite(0xb0,0);
}

void apicInit() {
    uacpi_table apic;
    uacpi_status ret = uacpi_table_find_by_signature("APIC",&apic);

    if(ret == UACPI_STATUS_OK) { 
        struct acpi_madt* apic_struct = ((struct acpi_madt*) apic.virt_addr);
        uint64_t lapic_base = rdmsr64(0x1b) | 0xfffff000;
        lapicVBase = (uint64_t)phys2Virt(lapic_base);
        pagingMap(phys2Virt(pagingGetKernel()),lapic_base,(uint64_t)phys2Virt(lapic_base),PTE_PRESENT | PTE_WRITABLE);
        printf("LAPIC: 0x%p\n",lapic_base);
        lapicTimerCalibrate();
        picDisable();
        lapicEnable(lapic_base);
        lapicWrite(0xF0,lapicRead(0xF0) | 0x100);

    } else {
        printf("APIC doesnt present by your firmware (%d)\nHalting kernel\n",ret);
    }

}