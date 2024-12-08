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

uint32_t* gopGetFramebuffer();

// Text

void textDrawChar(int xOffset, int yOffset, int colour, char ch);

void textDrawString(char* String,int X,int Y,int Color);

void textPrintChar(char key);

void textPutChar(int x,int y,char key);

void kernelInitTextMode();

void textClearChar(int x, int y);

void textPrintString(char* String);

void textClearTextScreen();

void textSetBG(uint32_t color);

void textSetFG(uint32_t color);

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