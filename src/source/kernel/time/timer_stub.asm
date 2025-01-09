
dummyInterruptContext:
    times 10 dq 0

extern timerHandler

global irq_32
irq_32:
    push rax
    mov ax,0x20
    out 0x20,ax
    pop rax
    iretq
    

section .bss
resb 65565
timer_stack:

