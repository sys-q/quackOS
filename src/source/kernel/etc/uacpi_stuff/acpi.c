
#include <kernelapi.h>
#include <uacpi/uacpi.h>
#include <uacpi/event.h>

void initACPI() {
    initUACPIHeap();
    uacpi_status ret = uacpi_initialize(0);
    ret = uacpi_namespace_load();
    asm volatile("int $90");
    ret = uacpi_namespace_initialize();
    ret = uacpi_finalize_gpe_initialization();
}