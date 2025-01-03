
dummyContext:
    times 25 dq 0

; rdi - interrupt_context_t*
extern processWork
global processSchedule
processSchedule:
    mov rax,[rdi]
    mov [dummyContext + 40],rax
    mov rax,[rdi + 8]
    mov [dummyContext + 48],rax
    mov rax,[rdi + 16]
    mov [dummyContext],rax
    mov rax,[rdi + 24]
    mov [dummyContext + 8],rax
    mov rax,[rdi + 32]
    mov [dummyContext + 16],rax
    mov rax,[rdi + 40]
    mov [dummyContext + 24],rax
    mov rax,[rdi + 48]
    mov [dummyContext + 32],rax
    mov rdi,dummyContext
    call contextSave
    jmp processWork


;rdi - dest
global contextSave
contextSave:
    mov [rdi + 56],rsi
    mov [rdi + 64],rdx
    mov [rdi + 72],rcx
    mov [rdi + 80],r8
    mov [rdi + 88],r9
    mov [rdi + 96],r10
    mov [rdi + 104],r11
    mov [rdi + 112],rbx
    mov [rdi + 120],rbp
    mov [rdi + 128],r12
    mov [rdi + 136],r13
    mov [rdi + 144],r14
    mov [rdi + 152],r15
    ret

; rdi - src
global contextSwitch
contextSwitch:
    mov rsi,[rdi + 56]
    mov rdx,[rdi + 64]
    mov rcx,[rdi + 72]
    mov r8,[rdi + 80]
    mov r9,[rdi + 88]
    mov r10,[rdi + 96]
    mov r11,[rdi + 104]
    mov rbx,[rdi + 112]
    mov rbp,[rdi + 120]
    mov r12,[rdi + 128]
    mov r13,[rdi + 136]
    mov r14,[rdi + 144]
    mov r15,[rdi + 152]
    push qword [rdi + 32]
    push qword [rdi + 24]
    push qword [rdi + 16]
    push qword [rdi + 8]
    push qword [rdi]
    push qword [rdi + 40]
    push qword [rdi + 48]
    mov rax,[rdi + 160]
    mov cr3,rax
    mov ax,0x20
    out 0x20,ax
    pop rdi
    pop rax
    iretq
