
isPITRunning db 0

%macro pit_irq 1
global pit_irq_%+%1
extern printf
pit_irq_%+%1:
    cli
    jmp pitIRQ
%endmacro

dummyContext:
    times 20 dq 0

extern pitHandler
extern processMainPaging
pitIRQ:
    cmp qword [rsp + 8],0x08
    jz .continue
    swapgs

 .continue:
    mov [dummyContext + 48],rdi
    mov [dummyContext + 40],rax
    mov rax,cr3
    mov [dummyContext + 56],rax
    call processMainPaging
    mov rdi,dummyContext
    mov [rdi + 64],rbx
    mov [rdi + 72],rdx
    mov [rdi + 80],rsi
    mov [rdi + 88],rbp
    mov [rdi + 96],r8
    mov [rdi + 104],r9
    mov [rdi + 112],r10
    mov [rdi + 120],r11
    mov [rdi + 128],r12
    mov [rdi + 136],r13
    mov [rdi + 144],r14
    mov [rdi + 152],r15
    ; save interrupt context 
    mov rax,[rsp]
    mov [rdi],rax
    mov rax,[rsp + 8]
    mov [rdi + 8],rax
    mov rax,[rsp + 16]
    mov [rdi + 16],rax
    mov rax,[rsp + 24]
    mov [rdi + 24],rax
    mov rax,[rsp + 32]
    mov [rdi + 32],rax
    sub rsp,40 ; clear stack from interrupt frame
    mov rsp,pit_stack
    call pitHandler
    mov rdi,r15 
    push qword [rdi + 32]
    push qword [rdi + 24]
    push qword [rdi + 16]
    push qword [rdi + 8]
    push qword [rdi]
    mov ax,0x20
    out 0x20,ax

    mov rax,[rdi + 56]
    mov cr3,rax
    mov rax,[rdi + 40]
    mov rbx,[rdi + 64]
    mov rdx,[rdi + 72]
    mov rsi,[rdi + 80]
    mov rbp,[rdi + 88]
    mov r8,[rdi + 96]
    mov r9,[rdi + 104]
    mov r10,[rdi + 112]
    mov r11,[rdi + 120]
    mov r12,[rdi + 128]
    mov r13,[rdi + 136]
    mov r14,[rdi + 144]
    mov r15,[rdi + 152]
    mov rdi,[rdi + 48]

    iretq

 .end:
    jmp pitIRQ

pit_irq 32


section .bss
resb 65565
pit_stack: