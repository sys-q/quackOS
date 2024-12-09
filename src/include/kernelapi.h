#include <stdint.h>
#include <limine.h>

// GOP

void gopPaint(uint32_t x,uint32_t y,uint32_t color);

void initGop();

void gopClear(uint32_t color);

void gopBackBuffer(uint32_t* backbuffer);

void gopSwap();

void gopFastEnable();

void gopFastDisable();

uint32_t gopGetWidth();

uint32_t gopGetHeight();

uint32_t gopGetPitch();

char gopGetFastState();

void gopDisableBackbuffer();

uint32_t* gopGetFramebuffer();

// Text

void textDrawChar(int xOffset, int yOffset, int colour, char ch);

void textDrawString(char* String,int X,int Y,int Color);

void textPrintChar(char key);

void textPutChar(int x,int y,char key);

void kernelInitTextMode();

void textClearChar(int x, int y);

void textPrintString(char* String);

void textClearTextScreen();

void textSetBG(uint32_t color);

void textSetFG(uint32_t color);

uint32_t textGetFG();

uint32_t textGetBG();

void textSetX(uint32_t x,uint32_t screenx);

void textSetY(uint32_t y,uint32_t screeny);

uint8_t getFontSizeX();

uint8_t getFontSizeY();

// CMOS

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71
#define CMOS_SECONDS 0x00
#define CMOS_MINUTES 0x02
#define CMOS_HOURS 0x04
#define CMOS_DAY 0x07
#define CMOS_MONTH 0x08
#define CMOS_YEAR 0x09

uint8_t cmosSecond();

uint8_t cmosMinute();

uint8_t cmosHour();

uint8_t cmosDay();

uint8_t cmosMonth();

uint16_t cmosYear();

void cmosSleep(int seconds);

void cmosWaitSecond();

// Random

uint64_t randNum(uint64_t min, uint64_t max);

// Log

void printf(char* format, ...);

void logPrintf(char* format, ...);

// GDT

typedef struct __attribute__((packed)) {
    uint16_t size;
    uint64_t base;
} gdt_pointer_t;

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint16_t baselow16;
    uint8_t basemid8;
    uint8_t access;
    uint8_t granularity;
    uint8_t basehigh8;
} gdt_entry_t;

typedef struct __attribute__((packed)) {
    uint16_t length;
    uint16_t baselow16;
    uint8_t basemid8;
    uint8_t flags0;
    uint8_t flags1;
    uint8_t basehigh8;
    uint32_t baseup32;
    uint32_t reserved;
} tss_entry_t;

typedef struct __attribute__((packed)) {
    uint32_t reserved0;
    uint64_t rsp[3];
    uint64_t reserved1;
    uint64_t ist[7];
    uint32_t reserved2;
    uint32_t reserved3;
    uint16_t reserved4;
    uint16_t iopb_offsset;
} tss_t;

typedef struct __attribute__((packed)) {
    gdt_entry_t zero;
    gdt_entry_t _64bitcode;
    gdt_entry_t _64bitdata;
    gdt_entry_t usercode;
    gdt_entry_t userdata;
    tss_entry_t tss;
} gdt_t;

extern void asmLoadGDT(uint64_t* pointer);

extern void asmLoadTSS();

void initGDT();

// IDT

typedef struct interrupt_frame {
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t vec;
    uint64_t error_code;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} interrupt_frame_t;

typedef struct __attribute__((packed)) {
    uint16_t baselow;
    uint16_t cs;
    uint8_t ist;
    uint8_t attr;
    uint16_t basemid;
    uint32_t basehigh;
    uint32_t reserved0;
} idt_entry_t;

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint64_t base;
} idtr_t;

void exceptionHandler(struct interrupt_frame frame);

void initIDT();

// PMM

typedef struct {
    uint64_t bits;
    uint64_t size;
} bitmap_t;

typedef struct {
    uint64_t* base;
    uint64_t length;
    uint64_t skip;
    bitmap_t bitmap;
} memory_entry_t;

