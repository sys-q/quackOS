
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
size_t strlen(const char *str);

void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
void outl(uint16_t port, uint32_t value);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t inl(uint16_t port);

void nop();
void cli();
void sti();
void hlt();
void outbwait();

uint64_t rdmsr64(uint32_t msr);

void wrmsr64(uint32_t msr, uint64_t value);

uint64_t rdtsc();

char* itoa(uint64_t value, char* str, int base );

uint64_t* virt2Phys(uint64_t address);

uint64_t* phys2Virt(uint64_t address);

void virtSetOffset(uint64_t ofset);

extern void gsSwitch();

void fpsTest();