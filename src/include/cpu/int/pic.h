#include <stdint.h>

#pragma once

void picRemap();

void picEOI(uint8_t irq);

void picSetMask(uint8_t irq);

void picClearMask(uint8_t irq);

void picDisable();

char picIsDisabled();

char picIsPitDisabled();