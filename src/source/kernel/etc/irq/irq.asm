

%macro irq 1
global irq_%+%1
irq_%+%1:
    cli
    mov rdi,%+%1
    call asmIRQ
    sti 
    iretq
%endmacro

extern irqHandler
asmIRQ:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    cmp qword [rsp + 8],0x08
    
    jz .continue
    swapgs
 .continue:
    
    cld

    call irqHandler

    cmp qword [rsp + 8],0x08
    jz .end
    swapgs

 .end:
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    ret

irq 32
