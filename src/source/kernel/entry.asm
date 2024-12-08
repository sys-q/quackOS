
global kernel_entry
extern kmain

kernel_entry:
    cli
    mov rsp,stack_space
    call kmain
    hlt

section .bss
resb 131072 ; 128 KB
stack_space: