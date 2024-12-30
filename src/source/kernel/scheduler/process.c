
#include <kernelapi.h>
#include <variables.h>
#include <driverbase.h>

process_t* head;
process_t* last;
process_t* current;
uint64_t last_id;
uint64_t process_count;
uint64_t process_ticks;

process_t* current_process() {
    return current;
}

process_t* processCreate(uint64_t paging_flags) {
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
    for(uint64_t i = ALIGNPAGEDOWN((uint64_t)virt2Phys(proc->context.rsp));i < (256 * PAGE_SIZE) + ALIGNPAGEUP((uint64_t)virt2Phys(proc->context.rsp));i += PAGE_SIZE) {
        vmmMapPage(virtcr3,i,(uint64_t)phys2Virt(i),paging_flags);
    }
    vmmMapBackBuffer(virtcr3);
    vmmMapPage(virtcr3,(uint64_t)virt2Phys((uint64_t)proc),(uint64_t)proc,paging_flags);
    //vmmMapEntry(virtcr3,LIMINE_MEMMAP_USABLE);
    last_id++;
    return proc;
}

process_t* processQueue(uint64_t rip,uint8_t is_user) {
    process_t* proc = head;
    while(1) {
        
        if(proc == 0 || proc == proc->next) {
            break;
        } else {    
            uint8_t usercs = 0x18 | 3;

            if(proc->status == PROCESS_FREE) {
                if(proc->context.cs == usercs && is_user) {
                    memset((uint64_t*)proc->start_rsp,0,256 * PAGE_SIZE);
                    uint64_t old_cr3 = proc->context.cr3;
                    memset(&proc->context,0,sizeof(process_context_t));
                    proc->context.cr3 = old_cr3;
                    proc->context.cs = 0x18 | 3;
                    proc->context.ss = 0x20 | 3;
                    proc->context.rflags = rflags();
                    proc->context.rip = rip;
                    proc->status = PROCESS_RUN;
                    proc->context.rsp = proc->start_rsp;
                    return proc;
                } else if(proc->context.cs != usercs && !is_user) {
                    memset((uint64_t*)proc->start_rsp,0,256 * PAGE_SIZE);
                    uint64_t old_cr3 = proc->context.cr3;
                    memset(&proc->context,0,sizeof(process_context_t));
                    proc->context.cr3 = old_cr3;
                    proc->context.cs = 0x08;
                    proc->context.ss = 0x10;
                    proc->context.rflags = rflags();
                    proc->context.rip = rip;
                    proc->status = PROCESS_RUN;
                    proc->context.rsp = proc->start_rsp;
                    return proc;
                }    
            }    
        } 
        proc = proc->next;  
         
    }    
        
    if(is_user) {
        proc = processCreate(PTE_PRESENT | PTE_WRITABLE | PTE_USER);
        proc->context.cs = 0x18 | 3;
        proc->context.ss = 0x20 | 3;
    } else {
        proc = processCreate(PTE_PRESENT | PTE_WRITABLE);
    }
    if(proc->id == 0) {
        last = proc;
        current = proc;
        head = proc;
    }
    proc->context.rip = rip;
    last->next = proc;
    last = proc;
    
    return proc;
}

void scheduling_lock() {
    picSetMask(0);
}

void scheduling_unlock() {
    picClearMask(0);
}

void processWork(process_context_t* last_context) {

    
    memcpy(&current->context,last_context,sizeof(process_context_t));

    

    if(current->next == 0)
            current = head;
    if(current->next->status != PROCESS_FREE) {
           asm volatile("mov %0, %%r15" : : "r" ((uint64_t)&current->next->context) : "memory");
           scheduling_unlock();
                   current = current->next;
           return;
    }
    processWork(last_context);
}

void processMainPaging() {
    vmmActivatePML(virt2Phys((uint64_t)vmmGetKernel())); // enable kernel paging
    return;
}
