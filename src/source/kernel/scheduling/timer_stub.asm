
dummyInterruptContext:
    times 10 dq 0

extern timerHandler

global irq_32
irq_32:
    cli
    mov [dummyInterruptContext],rax
    mov [dummyInterruptContext + 8],rdi
    mov rax,[rsp]
    mov [dummyInterruptContext + 16],rax
    mov rax,[rsp + 8]
    mov [dummyInterruptContext + 24],rax
    mov rax,[rsp + 16]
    mov [dummyInterruptContext + 32],rax
    mov rax,[rsp + 24]
    mov [dummyInterruptContext + 40],rax
    mov rax,[rsp + 32]
    mov [dummyInterruptContext + 48],rax
    mov rdi,dummyInterruptContext
    mov rsp,timer_stack
    jmp timerHandler  
    

section .bss
resb 65565
timer_stack:

