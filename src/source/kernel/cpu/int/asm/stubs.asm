
extern exceptionHandler

%macro isr_err_stub 1
global isr_%+%1
isr_%+%1:
    mov rsp,kernel_panic_stack
    mov rdi,%+%1
    jmp exceptionHandler
    hlt
%endmacro

%macro isr_no_err_stub 1
global isr_%+%1
isr_%+%1:
    mov rsp,kernel_panic_stack
    mov rdi,%+%1
    jmp exceptionHandler
    hlt
%endmacro

global dummyIRQ
dummyIRQ:
    iretq

isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub 8
isr_no_err_stub 9
isr_err_stub 10
isr_err_stub 11
isr_err_stub 12
isr_err_stub 13
isr_err_stub 14
isr_no_err_stub 16
isr_err_stub 17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_err_stub 21
isr_no_err_stub 28
isr_err_stub 29
isr_err_stub 30

section .bss
resb 65565
kernel_panic_stack: