
#include <stdint.h>
#include <scheduling/process.h>
#include <cpu/int/pic.h>
#include <memory/pmm.h>
#include <memory/paging.h>
#include <driverbase.h>
#include <variables.h>
#include <fthelper.h>
#include <limine.h>

uint64_t lastid = 0;
process_t* last;
process_t* head;
process_t* current;

process_t* processCreate(uint64_t paging_flags) {
    process_t* proc = (process_t*)pmmVZero();
    uint64_t rsp = (uint64_t)pmmVBigAlloc(PROCESS_STACK_SIZE);
    uint64_t phys_rsp = (uint64_t)virt2Phys(rsp);
    memset(&proc->ctx,0,sizeof(process_context_t));
    proc->ctx.rsp = rsp + (PROCESS_STACK_SIZE * PAGE_SIZE);
    proc->start_rsp = proc->ctx.rsp;
    proc->ctx.cs = 0x08;
    proc->ctx.ss = 0x10;
    proc->ctx.rflags = rflags();
    proc->ctx.cr3 = pmmZero();
    proc->status = PROCESS_STATUS_RUNNING;
    proc->id = lastid;
    lastid++;
    uint64_t* virtcr3 = phys2Virt(proc->ctx.cr3);
    pagingKernel(virtcr3);
    for(uint64_t i = 0; i <= PROCESS_STACK_SIZE;i++) {
        pagingMap(virtcr3,phys_rsp + (i * PAGE_SIZE),rsp + (i * PAGE_SIZE),paging_flags);
    }
    pagingMapEntry(virtcr3,LIMINE_MEMMAP_FRAMEBUFFER);
    return proc;

}

void processQueue(uint64_t rip,char user) {
    process_t* proc = processCreate(user ? PTE_WRITABLE | PTE_PRESENT | PTE_USER : PTE_WRITABLE | PTE_PRESENT);
    proc->ctx.cs = user ? 0x18 : 0x08;
    proc->ctx.ss = user ? 0x20 : 0x10;
    proc->ctx.rip = rip;
    if(proc->id == 0) {
        head = proc; 
        current = proc;
    }
    else 
        last->next = proc;
    last = proc;
}

void processScheduleTimerEnd() {
    if(!picIsDisabled()) {
        
    }
}

void processWork(process_context_t* ctx) {

    if(ctx) {
        current->ctx.rax = ctx->rax;
        current->ctx.rcx = ctx->rcx;
        current->ctx.rbx = ctx->rbx;
        current->ctx.rdx = ctx->rdx;
        current->ctx.rdi = ctx->rdi;
        current->ctx.rsi = ctx->rsi;
        current->ctx.r8 = ctx->r8;
        current->ctx.r9 = ctx->r9;
        current->ctx.r10 = ctx->r10;
        current->ctx.r11 = ctx->r11;
        current->ctx.r12 = ctx->r12;
        current->ctx.r13 = ctx->r13;
        current->ctx.r14 = ctx->r14;
        current->ctx.r15 = ctx->r15;
        current->ctx.rflags = ctx->rflags;
        current->ctx.rsp = ctx->rsp;
        current->ctx.rbp = ctx->rbp;
    }

    if(current->next == 0)
        current = head;
    
    if(current->next->status != PROCESS_STATUS_KILL) {
        current = current->next;
        processScheduleTimerEnd();
        contextSwitch(&current->ctx);
    }

    processSchedule(0);

}