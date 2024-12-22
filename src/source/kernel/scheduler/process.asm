
;uint64_t rip; [rdi]
;uint64_t rsp; [rdi +8]
;uint64_t rax; [rdi + 16]
;uint64_t rbx; [rdi + 24]
;uint64_t rcx; [rdi + 32]
;uint64_t rdx; [rdi + 40]
;uint64_t rsi; [rdi + 48]
;uint64_t rdi; [rdi + 56]
;uint64_t rbp; [rdi + 64]
;uint64_t r8; [rdi + 72]
;uint64_t r9; [rdi + 80]
;uint64_t r10; [rdi + 88]
;uint64_t r11; [rdi + 96]
;uint64_t r12; [rdi + 104]
;uint64_t r13; [rdi + 112]
;uint64_t r14; [rdi + 120]
;uint64_t r15; [rdi + 128]

newCR3 dq 0
oldRAX dq 0
contextRIP dq 100

global contextSwitch
contextSwitch:
    mov rax, [rdi]     
    mov [contextRIP], rax 
    mov rsp, [rdi + 8]    
    mov rax, [rdi + 136]   
    test rax, rax
    jz .continue
    swapgs           

.continue:
    mov rax, [rdi + 144]  
    mov cr3, rax          
    mov rax, [rdi + 16]   
    mov rbx, [rdi + 24]
    mov rcx, [rdi + 32]
    mov rdx, [rdi + 40]
    mov rsi, [rdi + 48]
    mov rbp, [rdi + 64]
    mov r8, [rdi + 72]
    mov r9, [rdi + 80]
    mov r10, [rdi + 88]
    mov r11, [rdi + 96]
    mov r12, [rdi + 104]
    mov r13, [rdi + 112]
    mov r14, [rdi + 120]
    mov r15, [rdi + 128]
    mov [oldRAX], rax    
    mov rdi, [rdi + 56]
    sti
    ret qword [contextRIP]

dummyContext:
    times 18 dq 0

global contextSave
contextSave:
    mov [dummyContext + 56],rdi
    mov [dummyContext + 16],rax
    mov rdi,dummyContext
    add rsp,8
    cmp qword [rsp + 8],0x08
    jz .continue
    mov rax,1
    mov [rdi + 136],rax
.continue:
    mov rax,[rsp]
    mov [rdi],rax
    mov [rdi + 8],rsp
    sub rsp,8
    mov [rdi + 24],rbx
    mov [rdi + 32],rcx
    mov [rdi + 40],rdx
    mov [rdi + 48],rsi
    mov [rdi + 64],rbp
    mov [rdi + 72],r8
    mov [rdi + 80],r9 
    mov [rdi + 88],r10 
    mov [rdi + 96],r11 
    mov [rdi + 104],r12 
    mov [rdi + 112],r13 
    mov [rdi + 120],r14 
    mov [rdi + 128],r15
    mov rax,cr3
    mov [rdi + 144],rax
    ret

global testasm
testasm:
    cli
    hlt