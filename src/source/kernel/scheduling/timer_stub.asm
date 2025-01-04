
dummyInterruptContext:
    times 10 dq 0

extern timerHandler

global irq_32
irq_32:
    iretq
    

section .bss
resb 65565
timer_stack:

