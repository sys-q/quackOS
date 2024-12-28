#include <kernelapi.h>
#include <driverbase.h>

char* exceptions[] = {
    [0] = "Division by zero",
    [1] = "Debug",
    [2] = "Non-maskable interrupt",
    [3] = "Breakpoint",
    [4] = "Overflow",
    [5] = "Bound range exceeded",
    [6] = "Invalid opcode",
    [7] = "Device not available",
    [8] = "Double fault",
    [9] = "Coprocessor Segment Overrun",
    [10] = "Invalid TSS",
    [11] = "Segment not present",
    [12] = "Stack-segment fault",
    [13] = "General protection fault",
    [14] = "Page fault",
    [16] = "x87 Floating-point exception",
    [17] = "Alignment check",
    [18] = "Machine check",
    [19] = "SIMD Floating-point exception",
    [20] = "Virtualization exception",
    [21] = "Control protection exception",
    [28] = "Hypervisor injection exception",
    [29] = "VMM communication exception",
    [30] = "Security exception"
};

void freezeKernel() {
    while(1) {
        hlt();
    }
}

void printDuck() {
    printf("\n");
    printf("                     _          _\n");
    printf("                     \\`.__..--'' `.\n");
    printf("                     ( _          ,\\\n");
    printf("                    ( <_< < <   `','`.\n");
    printf("                     \\ (_< < <    \\   `.\n");
    printf("                      `. `----'   (  q _p\n");
    printf("                        `-._  _.-' `-(_''\\\n");
    printf("                         (_'))--,      `._\\\n");
    printf("                            `-._<\n");
    printf("                                    \n");
    printf("\n\n");
}

uint64_t saveCR2() {
    uint64_t cr2;
    asm("mov %%cr2, %0" : "=r" (cr2));
    return cr2;
}

uint8_t isPanic = 0;

void exceptionHandler(struct interrupt_frame frame) {
    cli();
    scheduling_lock();

    if(isPanic) {
        printf(" Fault function : NOT RESOLVED\n");
        freezeKernel();
    } 
    isPanic = 1;
    textClearTextScreen();
    gopFastEnable();
    gopDisableBackbuffer();
    textSetBG(0);
    textSetFG(0xFFFFFF);
    vmmActivatePML(virt2Phys((uint64_t)vmmGetPMM()));
    //textClearTextScreen();
    printDuck();
    textSetX(25,24*getFontSizeX());
    printf("Kernel Panic\n\n");
    textSetX(25,24*getFontSizeX());
    printf("Information\n\n");
    printf(" RAX: 0x%p    RBX: 0x%p    RDX: 0x%p    RCX: 0x%p    RDI: 0x%p\n",frame.rax,frame.rbx,frame.rdx,frame.rcx,frame.rdi);
    printf(" RSI: 0x%p    R15: 0x%p    R14: 0x%p    R13: 0x%p    R12: 0x%p\n",frame.rsi,frame.r15,frame.r14,frame.r13,frame.r12);
    printf(" R11: 0x%p    R10: 0x%p    R9: 0x%p     R8: 0x%p      RBP: 0x%p\n",frame.r11,frame.r10,frame.r9,frame.r8,frame.rbp);
    printf(" RIP: 0x%p    SS: 0x%p     ERR: 0x%p    CS: 0x%p     RFLAGS: 0x%p\n",frame.rip,frame.ss,frame.error_code,frame.cs,frame.rflags);
    printf(" RSP: 0x%p    VEC: 0x%p    CR2: 0x%p     Exception: 0x%p\n",frame.rsp,frame.vec,saveCR2(),frame.r15);
    printf(" Text: %s\n",exceptions[frame.r15]);
    printf(" Fault function: %s()",disasmFunctionName(frame.rip));

    freezeKernel();
}