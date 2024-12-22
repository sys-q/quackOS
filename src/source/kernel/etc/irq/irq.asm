

%macro pit_irq 1
global pit_irq_%+%1
pit_irq_%+%1:
    cli
    jmp pitIRQ
%endmacro


extern contextSave
extern contextSwitch
extern pitHandler
pitIRQ:

    cmp qword [rsp + 8],0x08
    jz .continue
    swapgs

 .continue:

    cld

    call contextSave
    call pitHandler
    jmp contextSwitch

 .end:
    iretq

pit_irq 32
