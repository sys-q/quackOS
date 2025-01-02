

%macro isr 1
global isr_%+%1
isr_%+%1:
    cli
    mov r15,%+%1
    call asmExceptionHandler
%endmacro

extern fixed
extern exceptionHandler
asmExceptionHandler:
    cli 
    mov r14,0
    cmp qword [rsp + 8],0x08
    jz .continue
    swapgs
    mov r14,1
 .continue:
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rbp
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax
    cld

    mov rdi,rsp
    mov rsi,r14
    call exceptionHandler

 .end:    
    hlt
    jmp short .end 


isr 0
isr 1
isr 2
isr 3
isr 4
isr 5
isr 6
isr 7
isr 8
isr 9
isr 10
isr 11
isr 12
isr 13
isr 14
isr 15
isr 16
isr 17
isr 18
isr 19
isr 20
isr 30

oldExceptionSeg db 0 
