
#include <limine.h>
#include <stdint.h>
#include <cpu/smp.h>
#include <fthelper.h>
#include <cpu/int/apic.h>
#include <driverbase.h>
#include <cpu/gdt.h>
#include <cpu/int/idt.h>
#include <memory/paging.h>
#include <lock/spinlock.h>
#include <memory/heap.h>
#include <time/lapic_timer.h>
#include <cpu/int/apic.h>
#include <cpu/data.h>

__attribute__((used, section(".limine_requests")))
static volatile struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0
};

uint8_t lock_smp;

void smpCpuBoot(struct limine_smp_info* cpu) {
    spinlock_lock(&lock_smp);
    pagingActivateKernel();
    gdtInit();
    idtInit();
    fetchData()->smp_info = cpu;  
    lapicInit();
    if(fetchData()->smp_info->lapic_id != lapicID()) {
        printf("CPU %d Error: Wrong LAPIC ID\n");
        cli();
        hlt();
    } else {
        printf("CPU %d Booted !\n",cpu->processor_id);
    }
    spinlock_unlock(&lock_smp);
    sti();
    while(1) {
        hlt();
    }
}

void smpInit() {
    struct limine_smp_info* current_cpu;
    for(uint32_t i = 0; i < smp_request.response->cpu_count;i++) {
        current_cpu = smp_request.response->cpus[i];
        current_cpu->goto_address = smpCpuBoot;
    }
}