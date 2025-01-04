
#include <cpu/int/idt.h>
#include <cpu/int/pic.h>
#include <memory/paging.h>
#include <limine.h>
#include <driverbase.h>
#include <fthelper.h>

struct flanterm_ctx* ft_ctxA;
uint64_t last_task_id = 0;

void exceptionHandler(uint8_t vector) {
    //picSetMask(0);
    pagingActivateKernel();
    flantermHelperInit(ft_ctxA);
    printf("! Kernel panic !\n");
    printf("See qemu for details\n");
    printf("For details check GDB\n");
    hlt();
}

void setupPanicFlanterm(struct flanterm_ctx* fb) {
    ft_ctxA = fb;
}