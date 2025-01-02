#include <driverbase.h>
#include <kernelapi.h>
#include <stdint.h>

void picRemap() {
    uint8_t m1 = inb(0x21);
    uint8_t m2 = inb(0xA1);
    outb(0x20,0x11);
    outb(0xA0,0x11);
    outbwait();
    outb(0x21,0x20);
    outb(0xA1,0x28);
    outbwait();
    outb(0x21,0x04);
    outb(0xA1,0x02);
    outbwait();
    outb(0x21,0x01);
    outb(0xA1,0x01);
    outbwait();
    outb(0x21,m1);
    outb(0xA1,m2);
}

void picEOI(uint8_t irq) {
    if(irq>= 8)
        outb(0xA0,0x20);
    outb(0x20,0x20);
}

void picSetMask(uint8_t irq) {
    uint16_t port;
    uint8_t value;
    if(irq < 8)
        port = 0x21;
    else {
        port = 0xA1;
        irq -= 8;
    }
    value = inb(port) | (1 << irq);
    outb(port,value);
}

void picClearMask(uint8_t irq) {
    uint16_t port;
    uint8_t value;
    if(irq < 8)
        port = 0x21;
    else {
        port = 0xA1;
        irq -= 8;
    }
    value = inb(port) & ~(1 << irq);
    outb(port,value);
}