#define HPET_STATUS_32BIT 0
#define HPET_STATUS_64BIT 1
#define HPET_COUNT_SIZE_CAP (1 << 13)
#define HPET_CLOCK_PERIOD_LIMIT 0x05F5E100

#include <stdint.h>

uint64_t hpetCounter();

void hpetSleep(uint64_t usec);

void hpetInit();

uint64_t hpetNanoCounter();