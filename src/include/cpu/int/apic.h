
#pragma once

#include <stdint.h>

void lapicWrite(uint32_t reg,uint32_t value);

uint32_t lapicRead(uint32_t reg);

void lapicEnable();

uint32_t lapicID();

void lapicEOI();

uint64_t lapicBase();

void lapicCPUEnable();

void lapicInit();