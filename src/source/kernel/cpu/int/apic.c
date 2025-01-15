
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
#include <time/hpet.h>
#include <driverbase.h>
#include <cpu/int/idt.h>
#include <time/lapic_timer.h>
#include <cpu/data.h>

void lapicWrite(uint32_t reg,uint32_t value) {
    *(volatile uint32_t*)(lapicBase() + reg) = value;
}

uint32_t lapicRead(uint32_t reg) {
    return *(volatile uint32_t*)(lapicBase() + reg);
}

void lapicEnable() {
    wrmsr64(0x1b,rdmsr64(0x1b));
}

uint32_t lapicID() {
    return lapicRead(0x20) >> 24;
}

void lapicEOI() {
    lapicWrite(0xb0,0);
}

uint64_t lapicBase() {
    return (uint64_t)phys2Virt(rdmsr64(0x1b) & 0xfffff000);
}

void lapicCPUEnable() {
    lapicEnable();
    lapicWrite(0xF0,0xFF | 0x100);
}

uint32_t i1 = 0;
uint32_t i2 = 0;

void lapicInit() {
    uacpi_table apic;
    uacpi_status ret = uacpi_table_find_by_signature("APIC",&apic);

    if(ret == UACPI_STATUS_OK) { 
        pagingMap(phys2Virt(pagingGetKernel()),(uint64_t)virt2Phys(lapicBase()),lapicBase(),PTE_PRESENT | PTE_WRITABLE | PTE_CACHE_MMIO);
        lapicTimerEnable();
        lapicCPUEnable();
        printf("LAPIC Base: 0x%p 0x%p\n",lapicBase(),rdmsr64(0x1b));
    } else {
        printf("APIC doesnt present by your firmware (%d)\nHalting kernel\n",ret);
        cli();hlt();
    }

}