
extern timerHandler

global irq_32
irq_32:
    cli
    push rdi
    push rax
    mov rdi,rsp
    mov rsp,timer_stack
    call timerHandler
    hlt

section .bss
resb 65565
timer_stack:

