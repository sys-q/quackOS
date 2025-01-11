
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
#include <time/hpet.h>
#include <memory/heap.h>
#include <pci/pci.h>

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

uacpi_status uacpi_kernel_get_rsdp(uacpi_phys_addr *out_rsdp_address) {
    *out_rsdp_address = (uacpi_phys_addr)rsdp_request.response->address;
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_pci_device_open(
    uacpi_pci_address address, uacpi_handle *out_handle
) {
    uacpi_pci_address* pci = kmalloc(sizeof(uacpi_pci_address));
    pci[0] = address;
    *out_handle = (uacpi_handle)pci;
    return UACPI_STATUS_OK;
}
void uacpi_kernel_pci_device_close(uacpi_handle handle) {
    kfree(handle);
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
    uacpi_pci_address* pci = (uacpi_pci_address*)device;
    switch(byte_width) {
        case 1:
            *value = pci_read_config_byte(pci->bus,pci->device,pci->function,offset);
            break;
        case 2:
            *value = pci_read_config_word(pci->bus,pci->device,pci->function,offset);
            break;
        case 4:
            *value = pci_read_config_dword(pci->bus,pci->device,pci->function,offset);
            break;
        default:
            return UACPI_STATUS_INVALID_ARGUMENT;
    }
    return UACPI_STATUS_OK;
}
uacpi_status uacpi_kernel_pci_write(
    uacpi_handle device, uacpi_size offset,
    uacpi_u8 byte_width, uacpi_u64 value
) {
    uacpi_pci_address* pci = (uacpi_pci_address*)device;
    switch(byte_width) {
        case 1:
            pci_write_config_byte(pci->bus,pci->device,pci->function,offset,value);
            break;
        case 2:
            pci_write_config_word(pci->bus,pci->device,pci->function,offset,value);
            break;
        case 4:
            pci_write_config_dword(pci->bus,pci->device,pci->function,offset,value);
            break;
        default:
            return UACPI_STATUS_INVALID_ARGUMENT;
    }
    return UACPI_STATUS_OK;
}

/*
 * Map a SystemIO address at [base, base + len) and return a kernel-implemented
 * handle that can be used for reading and writing the IO range.
 */
uacpi_status uacpi_kernel_io_map(
    uacpi_io_addr base, uacpi_size len, uacpi_handle *out_handle
) {
    uint64_t* io_struct = kmalloc(16);
    io_struct[0] = base;
    io_struct[1] = len;
    *out_handle = (uacpi_handle*)io_struct;
    return UACPI_STATUS_OK;
}

void uacpi_kernel_io_unmap(uacpi_handle handle) {
    kfree(handle);
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
    return kmalloc(size);
}

void uacpi_kernel_free(void *mem) {
    kfree(mem);
}

void uacpi_kernel_log(uacpi_log_level, const uacpi_char* str) {
    printf("UACPI: %s",str);
}

uacpi_u64 uacpi_kernel_get_nanoseconds_since_boot(void) {
    return hpetNanoCounter() - boottime_request.response->boot_time;
}

void uacpi_kernel_stall(uacpi_u8 usec) {
    hpetSleep(usec);
}

void uacpi_kernel_sleep(uacpi_u64 msec) {
    hpetSleep(msec * 1000);
}

uacpi_handle uacpi_kernel_create_mutex(void) {
    uint8_t* spinlock = (uint8_t*)kmalloc(1);
    *spinlock = 0;
    return (uacpi_handle)spinlock;
}

void uacpi_kernel_free_mutex(uacpi_handle handle) {
    kfree(handle);
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
    spinlock_lock((uint8_t*)spinlock);
    return UACPI_STATUS_OK;
}

void uacpi_kernel_release_mutex(uacpi_handle spinlock) {
    spinlock_unlock((uint8_t*)spinlock);
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
    uint8_t* spinlock = (uint8_t*)kmalloc(1);
    *spinlock = 0;
    return (uacpi_handle)spinlock;
}

void uacpi_kernel_free_spinlock(uacpi_handle handle) {
    kfree(handle);
}

uacpi_cpu_flags uacpi_kernel_lock_spinlock(uacpi_handle spinlock) {
    spinlock_lock((uint8_t*)spinlock);
    return 1;
}
void uacpi_kernel_unlock_spinlock(uacpi_handle spinlock, uacpi_cpu_flags) {
    spinlock_unlock((uint8_t*)spinlock);
}

uacpi_status uacpi_kernel_schedule_work(
    uacpi_work_type, uacpi_work_handler, uacpi_handle ctx
) {
    return 0;
}

uacpi_status uacpi_kernel_wait_for_work_completion(void) {
    return 0;
}
