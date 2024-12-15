

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
    ret

irq 32
