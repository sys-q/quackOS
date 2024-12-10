#include <stdint.h>
#include <kernelapi.h>
#include <limine.h>

__attribute__((used, section(".limine_requests")))
static volatile struct limine_kernel_address_request kerneladdress_request = {
    .id=LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};