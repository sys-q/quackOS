#include <uacpi/kernel_api.h>
#include <kernelapi.h>
#include <variables.h>
#include <driverbase.h>
#include <limine.h>
#include <uacpi/types.h>
#include <uacpi/platform/arch_helpers.h>

__attribute__((used, section(".limine_requests")))
static volatile struct limine_boot_time_request boottime_request = {
    .id=LIMINE_BOOT_TIME_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_rsdp_request rsdp_request = {
    .id=LIMINE_RSDP_REQUEST,
    .revision = 0
};

uint64_t uacpi_heap;

void initUACPIHeap() {
    uacpi_heap = allocPages(2048); // 8 MB
    vmmActivatePML(virt2Phys((uint64_t)vmmGetPMM()));
    for(uint64_t i = uacpi_heap;i < uacpi_heap + 2048;i++) {
        vmmMapPage(vmmGetKernel(),(uint64_t)virt2Phys((uint64_t)pageToVirt(i)),(uint64_t)pageToVirt(i),PTE_PRESENT | PTE_WRITABLE);
    }
    vmmActivatePML(virt2Phys((uint64_t)vmmGetKernel()));
    initMultiHeapPage(uacpi_heap,2048);
}

uacpi_status uacpi_kernel_get_rsdp(uacpi_phys_addr *out_rsdp_address) {
    *out_rsdp_address = (uint64_t)rsdp_request.response->address;
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_pci_read(
    uacpi_pci_address *address, uacpi_size offset,
    uacpi_u8 byte_width, uacpi_u64 *value
) {
    return UACPI_STATUS_UNIMPLEMENTED;
}
uacpi_status uacpi_kernel_pci_write(
    uacpi_pci_address *address, uacpi_size offset,
    uacpi_u8 byte_width, uacpi_u64 value
) {
    return UACPI_STATUS_UNIMPLEMENTED;
}
 
// 0 - base
// 8 - len
uacpi_status uacpi_kernel_io_map(
    uacpi_io_addr base, uacpi_size len, uacpi_handle *out_handle
) {
    uint64_t* p_base = pageMalloc(uacpi_heap,16);
    *p_base = base;
    p_base[1] = len;
    *out_handle = (uacpi_handle)p_base;
    return UACPI_STATUS_OK;
}

void uacpi_kernel_io_unmap(uacpi_handle handle) {
    pageFree((void*)handle);
}

uacpi_status uacpi_kernel_io_read(
    uacpi_handle handle, uacpi_size offset,
    uacpi_u8 byte_width, uacpi_u64 *value
) {
    uint64_t* p_base = (uint64_t*)handle;
    if(offset > p_base[1])
        return UACPI_STATUS_INVALID_ARGUMENT;
    switch(byte_width) {
        case 1:
            *value = inb(p_base[0] + offset);
            break;
        case 2:
            *value = inw(p_base[0] + offset);
            break;
        case 4:
            *value = inl(p_base[0] + offset);
            break;
    }
    return UACPI_STATUS_OK;
}
uacpi_status uacpi_kernel_io_write(
    uacpi_handle handle, uacpi_size offset,
    uacpi_u8 byte_width, uacpi_u64 value
) {
    uint64_t* p_base = (uint64_t*)handle;
    if(offset > p_base[1])
        return UACPI_STATUS_INVALID_ARGUMENT;
    switch(byte_width) {
        case 1:
            outb(p_base[0] + offset,value);
            break;
        case 2:
            outw(p_base[0] + offset,value);
            break;
        case 4:
            outl(p_base[0] + offset,value);
            break;
    }
    return UACPI_STATUS_OK;
}

void *uacpi_kernel_map(uacpi_phys_addr addr, uacpi_size len) {
    vmmActivatePML(virt2Phys((uint64_t)vmmGetPMM()));
    uint64_t aligned_addr = ALIGNPAGEDOWN(addr);
    for(uint64_t i = aligned_addr;i < aligned_addr + len;i += PAGE_SIZE) {
        vmmMapPage(vmmGetKernel(),i,(uint64_t)phys2Virt(i),PTE_PRESENT | PTE_WRITABLE);
    }
    vmmActivatePML(virt2Phys((uint64_t)vmmGetKernel()));
    return phys2Virt(addr);
}
void uacpi_kernel_unmap(void *addr, uacpi_size len) {
    nop(); // unless for hhdm mapping
}

void *uacpi_kernel_alloc(uacpi_size size) {
    return pageMalloc(uacpi_heap,size);
}

void *uacpi_kernel_calloc(uacpi_size count, uacpi_size size) {
    return pageMalloc(uacpi_heap,count * size);
}

void uacpi_kernel_free(void *mem) {
    pageFree(mem);
}

void uacpi_kernel_log(uacpi_log_level lvl, const uacpi_char* str) {
    logPrintf("UACPI: %s\n",str);
}

uacpi_u64 uacpi_kernel_get_nanoseconds_since_boot(void) {
    return boottime_request.response->boot_time;
}

void uacpi_kernel_stall(uacpi_u8 usec) {
    
}

void uacpi_kernel_sleep(uacpi_u64 msec) {
    for(uint64_t i = pitCurrentTicks();i < pitCurrentTicks() + msec;i = pitCurrentTicks()) {
        nop();
    }
}

uacpi_handle uacpi_kernel_create_mutex(void) {
    atomic_flag* lock = pageMalloc(uacpi_heap,sizeof(atomic_flag));
    return lock;
}

void uacpi_kernel_free_mutex(uacpi_handle flag) {
    pageFree(flag);
}

uacpi_handle uacpi_kernel_create_event(void) {
    return 0;
}

void uacpi_kernel_free_event(uacpi_handle handle) {
    nop();
}

uacpi_thread_id uacpi_kernel_get_thread_id(void) {
    return 0;
}

uacpi_status uacpi_kernel_acquire_mutex(uacpi_handle flag, uacpi_u16 quack) {
    spinlock_lock((atomic_flag*)flag);
    return UACPI_STATUS_OK; 
}
void uacpi_kernel_release_mutex(uacpi_handle lock) {
    spinlock_unlock((atomic_flag*)lock);
}

uacpi_bool uacpi_kernel_wait_for_event(uacpi_handle, uacpi_u16) {
    return 1;
}

void uacpi_kernel_signal_event(uacpi_handle) {
    nop();
}


void uacpi_kernel_reset_event(uacpi_handle) {
    nop();
}


uacpi_status uacpi_kernel_handle_firmware_request(uacpi_firmware_request*) {
    return UACPI_STATUS_UNIMPLEMENTED;
}

uacpi_status uacpi_kernel_install_interrupt_handler(
    uacpi_u32 irq, uacpi_interrupt_handler handler, uacpi_handle ctx,
    uacpi_handle *out_irq_handle
) {
    uint8_t vector = allocIRQ();
    *out_irq_handle = (uacpi_handle)vector;
    idtSetDescriptor(vector,(void*)handler,0x8E);
    return UACPI_STATUS_OK;
}


uacpi_status uacpi_kernel_uninstall_interrupt_handler(
    uacpi_interrupt_handler, uacpi_handle irq_handle
) {
    return UACPI_STATUS_UNIMPLEMENTED;
}


uacpi_handle uacpi_kernel_create_spinlock(void) {
    atomic_flag* lock = pageMalloc(uacpi_heap,sizeof(atomic_flag));
    return (uacpi_handle)lock;
}
void uacpi_kernel_free_spinlock(uacpi_handle handle) {
    pageFree(handle);
}

uacpi_cpu_flags uacpi_kernel_lock_spinlock(uacpi_handle lock) {
    cli();
    spinlock_lock((atomic_flag*)lock);
    return 1;
}

void uacpi_kernel_unlock_spinlock(uacpi_handle lock, uacpi_cpu_flags) {
    spinlock_unlock((atomic_flag*)lock);
    sti();
}

uacpi_status uacpi_kernel_schedule_work(
    uacpi_work_type, uacpi_work_handler, uacpi_handle ctx
) {
    return UACPI_STATUS_UNIMPLEMENTED;
}


uacpi_status uacpi_kernel_wait_for_work_completion(void) {
    return UACPI_STATUS_UNIMPLEMENTED;
}