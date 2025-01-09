#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71
#define CMOS_SECONDS 0x00
#define CMOS_MINUTES 0x02
#define CMOS_HOURS 0x04
#define CMOS_DAY 0x07
#define CMOS_MONTH 0x08
#define CMOS_YEAR 0x09

#include <stdint.h>

uint8_t cmosSecond();

uint8_t cmosMinute();

uint8_t cmosHour();

uint8_t cmosDay();

uint8_t cmosMonth();

uint16_t cmosYear();

void cmosSleep(int seconds);

void cmosWaitSecond();