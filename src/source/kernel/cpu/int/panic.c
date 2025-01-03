
#include <cpu/int/idt.h>
#include <cpu/int/pic.h>
#include <scheduling/process.h>
#include <memory/paging.h>
#include <limine.h>
#include <driverbase.h>
#include <fthelper.h>

struct flanterm_ctx* ft_ctxA;

void exceptionHandler(uint8_t vector) {
    picSetMask(0);
    pagingActivateKernel();
    flantermHelperInit(ft_ctxA);
    process_t* proc = currentProcess();
    if(proc != 0) {
        printf("Process exception: ID: %d, RIP: 0x%p Error: 0x%p\n",proc->id,proc->ctx.rip,vector);
        proc->status = PROCESS_STATUS_KILL;
        proc->ctx.rip = 0;
        proc->ctx.rsp = proc->start_rsp;
        processWork(0);
        hlt();
    }
    printf("! Kernel panic !\n");
    printf("See qemu for details\n");
    printf("For details check GDB\n");
    hlt();
}

void setupPanicFlanterm(struct flanterm_ctx* fb) {
    ft_ctxA = fb;
}