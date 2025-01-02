#include <stdint.h>
#include <driverbase.h>
#include <kernelapi.h>

// CMOS wait second and etc will not work with PIT and etc

static inline uint8_t cmos_read(uint8_t reg) {
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

static inline uint8_t bcd_to_binary(uint8_t bcd) {
    return (bcd & 0x0F) + ((bcd / 16) * 10);
}

uint8_t cmosSecond() {
    uint8_t seconds = cmos_read(CMOS_SECONDS);
    return bcd_to_binary(seconds);
}

uint8_t cmosMinute() {
    uint8_t minutes = cmos_read(CMOS_MINUTES);
    return bcd_to_binary(minutes);
}

uint8_t cmosHour() {
    uint8_t hours = cmos_read(CMOS_HOURS);
    return bcd_to_binary(hours);
}

uint8_t cmosDay() {
    uint8_t day = cmos_read(CMOS_DAY);
    return bcd_to_binary(day);
}

uint8_t cmosMonth() {
    uint8_t month = cmos_read(CMOS_MONTH);
    return bcd_to_binary(month);
}

uint16_t cmosYear() {
    uint8_t year = cmos_read(CMOS_YEAR);
    return 2000 + bcd_to_binary(year); 
}

void cmosSleep(int seconds){
    for(int i =0; i< seconds;i++) {
        cmosWaitSecond();
    }
}

void cmosWaitSecond() {
    uint8_t last = cmosSecond();
    while(cmosSecond() == last) {
        nop();
    }
}