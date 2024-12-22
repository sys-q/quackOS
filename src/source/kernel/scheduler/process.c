
#include <kernelapi.h>
#include <variables.h>
#include <driverbase.h>

process_t* head;
process_t* last;
process_t* current;
uint64_t last_id;

process_t* processCreate() {
    process_t* proc = (process_t*)phys2Virt(allocZeroPagePhys());
    proc->status = PROCESS_RUN;
    proc->next = 0;
    proc->id = last_id;
    proc->context.rsp = (uint64_t)pageToVirt(allocPages(256));
    proc->context.cr3 = allocZeroPagePhys();
    uint64_t* virt_cr3 = phys2Virt(proc->context.cr3);
    vmmMapKernel(virt_cr3);
    vmmMapEntry(virt_cr3,LIMINE_MEMMAP_USABLE);
    vmmMapBackBuffer(virt_cr3);
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
    process_t* proc = head;
    current->context.cr3 = last_context->cr3;
    current->context.r10 = last_context->r10;
    current->context.r11 = last_context->r11;
    current->context.r12 = last_context->r12;
    current->context.r13 = last_context->r13;
    current->context.r14 = last_context->r14;
    current->context.r15 = last_context->r15;
    current->context.rax = last_context->rax;
    current->context.rbx = last_context->rbx;
    current->context.rcx = last_context->rcx;
    current->context.rdx = last_context->rdx;
    current->context.rdi = last_context->rdi;
    current->context.rsi = last_context->rsi;
    current->context.rip = last_context->rip;
    current->context.rsp = last_context->rsp;
    current->context.rbp = last_context->rbp;
    current->context.r8 = last_context->r8;
    current->context.r9 = last_context->r9;
    current->context.user = last_context->user;

    while(true) {

        if(proc->next != 0 && proc->next->id > current->id) {
            if(proc->next->status != PROCESS_FREE) {
                current = proc;
                picEOI(0);
                asm volatile("mov %0, %%rdi" : : "r" ((uint64_t)&proc->next->context) : "memory");
                return;
            }
        } 
        
        if(proc->next == 0) {
            current = head;
            proc = head;
        } else {
            proc = proc->next;
        }

    }
}