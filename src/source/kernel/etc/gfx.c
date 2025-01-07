
#include <stdint.h>
#include <etc/gfx.h>

uint32_t* fb_ptr;
uint32_t fb_pitch;

void pixelDraw(uint32_t x,uint32_t y,uint32_t color) {
    fb_ptr[y * (fb_pitch / 4) + x]  = color;
}

void gfxInit(uint32_t* fb,uint32_t pitch) {
    fb_ptr = fb;
    fb_pitch = pitch;
}