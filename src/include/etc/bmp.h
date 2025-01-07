
#include <stdint.h>


typedef struct __attribute__((packed)) {
    uint16_t type;
    uint32_t size;
    uint16_t r1;
    uint16_t r2;
    uint32_t offset;
} bmp_header;

typedef struct __attribute__((packed)) {
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t size_image;
    uint32_t xpelspermeter;
    uint32_t ypelspermeter;
    uint32_t clrused;
    uint32_t clrimportant;
} bmp_info;

bmp_info* parseBMP(uint64_t addr);

void drawBMP(uint64_t x,uint64_t y,uint64_t addr);