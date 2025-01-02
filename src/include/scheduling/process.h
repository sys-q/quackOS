
#include <stdint.h>

#define PROCESS_STATUS_KILL 0
#define PROCESS_STATUS_RUNNING 1

typedef struct process_context {
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
    uint64_t rax;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t rbx;
    uint64_t rbp;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t cr3;
} __attribute__((packed)) process_context_t;

typedef struct interrupt_context {
    uint64_t rax; //save
    uint64_t rdi; //save
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed)) interrupt_context_t;

typedef struct process {
    uint8_t status;
    uint64_t id;
    uint64_t start_rsp;
    process_context_t ctx;
    struct process* next;
} __attribute__((packed)) process_t;
