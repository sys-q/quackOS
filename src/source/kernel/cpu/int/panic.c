
#include <cpu/int/idt.h>
#include <limine.h>
#include <driverbase.h>
#include <fthelper.h>

struct flanterm_ctx* ft_ctxA;

void exceptionHandler(error_frame_t* err_frame) {
    flantermHelperInit(ft_ctxA);
    printf("! Kernel panic !\n");
    printf("Vector: 0x%p, RIP: 0x%p, error code: 0x%p\n",err_frame->vector,err_frame->rip,err_frame->error_code);
    printf("For details check GDB (info symbol [RIP])\n");
    hlt();
}

void setupPanicFlanterm(struct flanterm_ctx* fb) {
    ft_ctxA = fb;
}