
#include <kernelapi.h>
#include <variables.h>
#include <driverbase.h>

process_t* head;
process_t* last;
process_t* current;
uint64_t last_id;
uint64_t process_count;
uint64_t process_ticks;

process_t* processCreate() {
    process_t* proc = (process_t*)phys2Virt(allocZeroPagePhys());
    proc->status = PROCESS_RUN;
    proc->next = 0;
    proc->id = last_id;
    proc->context.rsp = (uint64_t)pageToVirt(allocPages(256));
    proc->context.rflags = rflags();
    proc->context.cs = 0x08;
    proc->context.ss = 0x10;
    proc->context.cr3 = allocZeroPagePhys();
    uint64_t* virtcr3 = phys2Virt(proc->context.cr3);
    vmmActivatePML(virt2Phys((uint64_t)vmmGetPMM()));
    vmmMapKernel(virtcr3);
    uint64_t flags = PTE_PRESENT | PTE_WRITABLE;
    for(uint64_t i = ALIGNPAGEDOWN((uint64_t)virt2Phys(proc->context.rsp));i < (256 * PAGE_SIZE) + ALIGNPAGEUP((uint64_t)virt2Phys(proc->context.rsp));i += PAGE_SIZE) {
        vmmMapPage(virtcr3,i,(uint64_t)phys2Virt(i),flags);
    }
    vmmMapBackBuffer(virtcr3);
    vmmMapPage(virtcr3,(uint64_t)virt2Phys((uint64_t)proc),(uint64_t)proc,flags);
    //vmmMapEntry(virtcr3,LIMINE_MEMMAP_USABLE);
    last_id++;
    return proc;
}

void processQueue(uint64_t rip) {
    process_t* proc = processCreate();
    if(proc->id == 0) {
        last = proc;
        current = proc;
        head = proc;
    }
    proc->context.rip = rip;
    last->next = proc;
    last = proc;
}

void processWork(process_context_t* last_context) {

    current->context.rip = last_context->rip;
    current->context.cs = last_context->cs;
    current->context.rflags = last_context->rflags;
    current->context.rsp = last_context->rsp;
    current->context.ss = last_context->ss;
    current->context.rax = last_context->rax;
    current->context.rdi = last_context->rdi;
    current->context.cr3 = last_context->cr3;
    current->context.rbx = last_context->rbx;
    current->context.rdx = last_context->rdx;
    current->context.rsi = last_context->rsi;
    current->context.rbp = last_context->rbp;
    current->context.r8 = last_context->r8;
    current->context.r9 = last_context->r9;
    current->context.r10 = last_context->r10;
    current->context.r11 = last_context->r11;
    current->context.r12 = last_context->r12;
    current->context.r13 = last_context->r13;
    current->context.r14 = last_context->r14;
    current->context.r15 = last_context->r15;


    if(current->next == 0)
            current = head;
    if(current->next->status != PROCESS_FREE) {
           asm volatile("mov %0, %%r15" : : "r" ((uint64_t)&current->next->context) : "memory");
                   current = current->next;
           return;
    }
    processWork(last_context);
}

void processMainPaging() {
    vmmActivatePML(virt2Phys((uint64_t)vmmGetPMM()));
    return;
}