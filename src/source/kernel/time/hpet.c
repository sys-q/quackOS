
#include <stdint.h>
#include <time/hpet.h>
#include <uacpi/acpi.h>
#include <uacpi/uacpi.h>
#include <uacpi/tables.h>
#include <uacpi/status.h>
#include <driverbase.h>
#include <memory/paging.h>

uint64_t hpetVBase;
uint32_t hpetClockPeriod;
uint32_t hpetClockPeriod_nano;
uint8_t hpetStatus;

uint64_t hpetCounter() {
    uint64_t pointer = hpetVBase + 0xf0; 
    return hpetStatus ? *(uint64_t*)pointer : *(uint32_t*)pointer;
}

void hpetSleep(uint64_t usec) {
    uint64_t start = hpetCounter();
    uint64_t nanoseconds = usec * 1000; // microseconds to nanoseconds
    while((hpetCounter() - start) * hpetClockPeriod_nano < nanoseconds) { 
        nop();
    }
}

void hpetInit() {
    uacpi_table hpet;
    uacpi_status ret = uacpi_table_find_by_signature("HPET",&hpet);
    if(ret == UACPI_STATUS_OK) {
        struct acpi_hpet* hpet_struct = ((struct acpi_hpet*)hpet.virt_addr);
        uint64_t minimal_tick = hpet_struct->min_clock_tick;
        hpetVBase = (uint64_t)phys2Virt(hpet_struct->address.address);
        pagingMap(phys2Virt(pagingGetKernel()),(uint64_t)virt2Phys(hpetVBase),hpetVBase,PTE_PRESENT | PTE_WRITABLE);
        uint64_t* hpet_config = (uint64_t*)(hpetVBase + 0x10);
        *hpet_config |= 1;

        hpetStatus = (*(uint64_t*)hpetVBase & HPET_COUNT_SIZE_CAP) ? HPET_STATUS_64BIT : HPET_STATUS_32BIT;

        hpetClockPeriod = *(uint32_t*)(hpetVBase + sizeof(uint32_t)); // clock period is 63:32 
        hpetClockPeriod_nano = hpetClockPeriod / 1000000; // convert femtoseconds to nanoseconds because if i convert clock period (femtoseconds) to microsecond, it will be <0

        printf("%d HPET: VBase: 0x%p ClockPeriod: 0x%p\n",hpetStatus ? 64 : 32,hpetVBase,hpetClockPeriod);


    } else {
        printf("HPET doesnt present by your firmware (%d)\nHalting kernel\n",ret);
        cli();
        hlt();
    }
}