#include <stdint.h>

void picRemap();

void picEOI(uint8_t irq);

void picSetMask(uint8_t irq);

void picClearMask(uint8_t irq);

char picIsDisabled();

char picIsPitDisabled();