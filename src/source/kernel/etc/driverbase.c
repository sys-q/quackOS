#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <driverbase.h>

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if (src > dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

int16_t strcmp(const char *str1, const char *str2) {
    while (*str1 && *str2 && *str1 == *str2) {
        if(*str1 == '\0')
            return 0;
        str1++;
        str2++;
    }
    return *str1 - *str2;
}

uint64_t rdmsr64(uint32_t msr) {
    uint32_t lo, hi;
    __asm__ volatile ("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr));
    return ((uint64_t)hi << 32) | lo; 
}

void wrmsr64(uint32_t msr, uint64_t value) {
    uint32_t lo = (uint32_t)(value & 0xFFFFFFFF); 
    uint32_t hi = (uint32_t)(value >> 32);
    __asm__ volatile ("wrmsr" : : "c"(msr), "a"(lo), "d"(hi));
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

void nop() {
    __asm__ volatile ("nop");
}

void cli() {
    __asm__ volatile ("cli");
}

void sti() {
    __asm__ volatile ("sti");
}

void hlt() {
    __asm__ volatile ("hlt");
}

uint64_t rflags() {
    uint64_t rflags;
    __asm__ volatile (
        "pushfq\n\t"      // Сохраняем RFLAGS на стек
        "pop %0"          // Извлекаем значение из стека в переменную
        : "=r" (rflags)   // Выходной операнд
    );
    return rflags;        // Возвращаем сохраненное значение RFLAGS
}

uint64_t cs() {
    uint64_t cs;
    asm volatile("mov %%cs, %0" : : "r" ((uint64_t)cs) : "memory");
    return cs;        // Возвращаем сохраненное значение RFLAGS
}

uint64_t ss() {
    uint64_t ss;
    asm volatile("mov %%ss, %0" : : "r" ((uint64_t)ss) : "memory");
    return ss;        // Возвращаем сохраненное значение RFLAGS
}


uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ volatile ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void outw(uint16_t port, uint16_t value) {
    __asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}

uint16_t inw(uint16_t port) {
    uint16_t value;
    __asm__ volatile ("inw %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void outl(uint16_t port, uint32_t value) {
    __asm__ volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
}

uint32_t inl(uint16_t port) {
    uint32_t value;
    __asm__ volatile ("inl %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

size_t strlen(const char *str) {
    const char *s = str;
    while (*s) {
        ++s;
    }
    return s - str;
}

void outbwait() {
    outb(0x80, 0);
}

char* itoa(uint64_t value, char* str, int base ) {
    char * rc;
    char * ptr;
    char * low;
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    low = ptr;
    do
    {
        *ptr++ = "ZYXWVUTSRQPONMLKJIHGFEDCBA9876543210123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[35 + value % base];
        value /= base;
    } while ( value );
    *ptr-- = '\0';
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}

uint64_t offset = 0;

uint64_t* virt2Phys(uint64_t address) {
    return (uint64_t*)((uint64_t)address - offset);
}

uint64_t* phys2Virt(uint64_t address) {
    return (uint64_t*)((uint64_t)address + offset);
}

void virtSetOffset(uint64_t ofset) {
    offset = ofset;
}