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

void textDrawChar(int xOffset, int yOffset, uint32_t color, char ch);

void textDrawString(char* String,int X,int Y,uint32_t Color);

void textDrawStringClear(char* String,int X,int Y,uint32_t Color);

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

void textClearCharGFX(int x, int y);

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

void idtSetDescriptor(uint8_t index,void* isr,uint8_t flags);

void initIDT();

void loadIDT();

// PMM

typedef struct {
    uint64_t bits;
    uint64_t size;
    uint64_t next;
    uint64_t num_pages;
    uint64_t pages_count;
} bitmap_t;

typedef struct {
    uint64_t base;
    uint64_t length;
    uint64_t skip;
    bitmap_t bitmap;
} memory_entry_t;

void bitmapSetBit(bitmap_t *bmp, uint64_t index);

void bitmapClearBit(bitmap_t *bmp, uint64_t index);

int bitmapIsBitSet(bitmap_t *bmp, uint64_t index);

uint64_t allocPage();

void freePage(uint64_t page);

uint64_t allocPages(uint64_t num_pages);

void freePages(uint64_t start_page, uint64_t num_pages);

uint64_t sizeToPages(uint64_t size);

struct limine_memmap_request* getMemMap();

memory_entry_t* getBiggestEntry();

uint64_t* earlyMalloc(uint64_t size);

bitmap_t bitmapInit(uint64_t size);

uint64_t allocZeroPage();

uint64_t allocZeroPagePhys();

void initPMM();

// VMM

#define PML4 0x01
#define PDP 0x02
#define PD 0x03
#define PT 0x04
#define ROUNDUP(VALUE,ROUND) ((VALUE + (ROUND - 1)) / ROUND)
#define ALIGNPAGEUP(VALUE) (ROUNDUP(VALUE,PAGE_SIZE) * PAGE_SIZE)
#define ALIGNPAGEDOWN(VALUE) ((VALUE / PAGE_SIZE) * PAGE_SIZE)
#define PTE_MASK_VALUE 0x000ffffffffff000
#define PTE_MASK(VALUE,FLAGS) (VALUE | FLAGS)
#define PTE_UNMASK(VALUE) (VALUE & PTE_MASK_VALUE)
#define PTE_PRESENT (1 << 0)
#define PTE_WRITABLE (1 << 1)
#define PTE_USER (1 << 2)
#define PTE_KERNELFLAGS PTE_PRESENT | PTE_WRITABLE
#define PTE_USERFLAGS PTE_PRESENT | PTE_WRITABLE | PTE_USER
#define PTE_READONLYKERNEL PTE_PRESENT

uint16_t vmmIndex(uint64_t virtual_address,uint8_t type);

void vmmMapPage(uint64_t* pml4,uint64_t phys,uint64_t virt,uint64_t flags);

void vmmMapKernel(uint64_t* pml4);

void vmmMapEntry(uint64_t* pml4,uint16_t type);

void vmmActivatePML(uint64_t* phys_pml);

void initVMM();

// PIC

void picRemap();

void picEOI(uint8_t irq);

void picSetMask(uint8_t irq);

void picClearMask(uint8_t irq);

// IRQ

void irqSetupHandler(uint16_t vector, void (*func)(uint16_t));

// PIT

void initPIT(uint32_t freq);

uint64_t pitCurrentTicks();