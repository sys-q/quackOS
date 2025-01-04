
#include <uacpi/types.h>
#include <uacpi/platform/arch_helpers.h>
#include <memory/paging.h>
#include <memory/pmm.h>
#include <uacpi/kernel_api.h>
#include <cpu/int/idt.h>
#include <etc/spinlock.h>
#include <driverbase.h>
#include <variables.h>  
#include <limine.h>
#include <fthelper.h>
#include <stdatomic.h>

__attribute__((used, section(".limine_requests")))
static volatile struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_boot_time_request boottime_request = {
    .id = LIMINE_BOOT_TIME_REQUEST,
    .revision = 0
};

uint64_t ptr = 0;

// its most lazy alloc in the world
void* easy_uacpi_alloc(uint64_t size) {
    if(!ptr) {
        ptr = (uint64_t)pmmVBigAlloc(2048 * 2);
    }
    uint64_t align_size;
    align_size = (size + 7) / 8 * 8;
    ptr += align_size;
    return (void*)ptr - align_size;
}

uacpi_status uacpi_kernel_get_rsdp(uacpi_phys_addr *out_rsdp_address) {
    *out_rsdp_address = (uacpi_phys_addr)rsdp_request.response->address;
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_pci_device_open(
    uacpi_pci_address address, uacpi_handle *out_handle
) {
    return UACPI_STATUS_UNIMPLEMENTED;
}
void uacpi_kernel_pci_device_close(uacpi_handle) {
    nop();
}

/*
 * Read & write the configuration space of a previously open PCI device.
 *
 * NOTE:
 * 'byte_width' is ALWAYS one of 1, 2, 4. Since PCI registers are 32 bits wide
 * this must be able to handle e.g. a 1-byte access by reading at the nearest
 * 4-byte aligned offset below, then masking the value to select the target
 * byte.
 */
uacpi_status uacpi_kernel_pci_read(
    uacpi_handle device, uacpi_size offset,
    uacpi_u8 byte_width, uacpi_u64 *value
) {
    return UACPI_STATUS_UNIMPLEMENTED;
}
uacpi_status uacpi_kernel_pci_write(
    uacpi_handle device, uacpi_size offset,
    uacpi_u8 byte_width, uacpi_u64 value
) {
    return UACPI_STATUS_UNIMPLEMENTED;
}

/*
 * Map a SystemIO address at [base, base + len) and return a kernel-implemented
 * handle that can be used for reading and writing the IO range.
 */
uacpi_status uacpi_kernel_io_map(
    uacpi_io_addr base, uacpi_size len, uacpi_handle *out_handle
) {
    uint64_t* io_struct = easy_uacpi_alloc(16);
    io_struct[0] = base;
    io_struct[1] = len;
    *out_handle = (uacpi_handle*)io_struct;
    return UACPI_STATUS_OK;
}

void uacpi_kernel_io_unmap(uacpi_handle handle) {
    nop();
}

/*
 * Read/Write the IO range mapped via uacpi_kernel_io_map
 * at a 0-based 'offset' within the range.
 *
 * NOTE:
 * 'byte_width' is ALWAYS one of 1, 2, 4. You are NOT allowed to break e.g. a
 * 4-byte access into four 1-byte accesses. Hardware ALWAYS expects accesses to
 * be of the exact width.
 */
uacpi_status uacpi_kernel_io_read(
    uacpi_handle handle, uacpi_size offset,
    uacpi_u8 byte_width, uacpi_u64 *value
) {
    uint64_t* io_sturct = (uint64_t*)handle;
    if(offset >=io_sturct[1])
        return UACPI_STATUS_INVALID_ARGUMENT;
    switch(byte_width) {
        case 1:
            *value = inb(io_sturct[0] + offset);
            break;
        case 2:
            *value = inw(io_sturct[0] + offset);
            break;
        case 4:
            *value = inl(io_sturct[0] + offset);
            break;
    }
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_write(
    uacpi_handle handle, uacpi_size offset,
    uacpi_u8 byte_width, uacpi_u64 value
) {
    uint64_t* io_sturct = (uint64_t*)handle;
    if(offset >=io_sturct[1])
        return UACPI_STATUS_INVALID_ARGUMENT;
    switch(byte_width) {
        case 1:
            outb(io_sturct[0] + offset,value);
            break;
        case 2:
            outw(io_sturct[0] + offset,value);
            break;
        case 4:
            outl(io_sturct[0] + offset,value);
            break;
    }
    return UACPI_STATUS_OK;
}

void *uacpi_kernel_map(uacpi_phys_addr addr, uacpi_size len) {
    uint64_t aligned_phys = ALIGNPAGEDOWN(addr);
    for(uint64_t i = aligned_phys;i < aligned_phys + len;i+= PAGE_SIZE) {
        pagingMap(phys2Virt(pagingGetKernel()),i,(uint64_t)phys2Virt(i),PTE_PRESENT | PTE_WRITABLE);
    }
    return (void*)phys2Virt(addr);
}

void uacpi_kernel_unmap(void *addr, uacpi_size len) {
    nop(); //hhdm doesnt need unmap
}

void *uacpi_kernel_alloc(uacpi_size size) {
    return easy_uacpi_alloc(size);
}

void uacpi_kernel_free(void *mem) {
    nop();
}

void uacpi_kernel_log(uacpi_log_level, const uacpi_char* str) {
    printf("UACPI: %s\n",str);
}

uacpi_u64 uacpi_kernel_get_nanoseconds_since_boot(void) {
    return boottime_request.response->boot_time; // i'll change it later
}

void uacpi_kernel_stall(uacpi_u8 usec) {
    nop();
}

void uacpi_kernel_sleep(uacpi_u64 msec) {
    for(uint64_t ticks = 0;ticks < msec;ticks++) {
        hlt();
    }
}

uacpi_handle uacpi_kernel_create_mutex(void) {
    atomic_flag* spinlock = easy_uacpi_alloc(sizeof(atomic_flag));
    return (uacpi_handle)spinlock;
}

void uacpi_kernel_free_mutex(uacpi_handle) {
    nop();
}

uacpi_handle uacpi_kernel_create_event(void) {
    return (uacpi_handle)0;
}

void uacpi_kernel_free_event(uacpi_handle) {
    nop();
}

uacpi_thread_id uacpi_kernel_get_thread_id(void) {
    return 0;
}

uacpi_status uacpi_kernel_acquire_mutex(uacpi_handle spinlock, uacpi_u16) {
    spinlock_lock((atomic_flag*)spinlock);
    return UACPI_STATUS_OK;
}

void uacpi_kernel_release_mutex(uacpi_handle spinlock) {
    spinlock_unlock((atomic_flag*)spinlock);
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
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_install_interrupt_handler(
    uacpi_u32 irq, uacpi_interrupt_handler m, uacpi_handle ctx,
    uacpi_handle *out_irq_handle
) {
    uint32_t vec = allocIRQ();
    idtSetDescriptor(vec,m,0x8E);
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_uninstall_interrupt_handler(
    uacpi_interrupt_handler, uacpi_handle irq_handle
) {
    return UACPI_STATUS_UNIMPLEMENTED;
}

uacpi_handle uacpi_kernel_create_spinlock(void) {
    atomic_flag* spinlock = (atomic_flag*)easy_uacpi_alloc(sizeof(atomic_flag));
    return (uacpi_handle)spinlock;
}

void uacpi_kernel_free_spinlock(uacpi_handle) {
    nop();
}

uacpi_cpu_flags uacpi_kernel_lock_spinlock(uacpi_handle spinlock) {
    spinlock_lock((atomic_flag*)spinlock);
    return 0;
}
void uacpi_kernel_unlock_spinlock(uacpi_handle spinlock, uacpi_cpu_flags) {
    spinlock_unlock((atomic_flag*)spinlock);
}

uacpi_status uacpi_kernel_schedule_work(
    uacpi_work_type, uacpi_work_handler, uacpi_handle ctx
) {
    return 0;
}

uacpi_status uacpi_kernel_wait_for_work_completion(void) {
    return 0;
}
