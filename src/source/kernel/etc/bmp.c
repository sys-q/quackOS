
#include <stdint.h>
#include <etc/gfx.h>
#include <etc/bmp.h>
#include <fthelper.h>

bmp_info* parseBMP(uint64_t addr) {
    return (bmp_info*)(addr + sizeof(bmp_header));
}

void drawBMP(uint64_t xA,uint64_t yB,uint64_t addr) {
    bmp_header* bmp_img = (bmp_header*)addr;
    if(bmp_img->type != 0x4D42) {
        printf("Image is not correct .bmp file\n");
    } else {
        bmp_info* bmp_infoA = (bmp_info*)(addr + sizeof(bmp_header));
        uint8_t* bmp_start = (uint8_t*)(addr + bmp_img->offset);
        uint64_t rowSize = (bmp_infoA->width * 3 + 3) & ~3;
        for(uint64_t y = 0; y < bmp_infoA->height; y++) {
            uint64_t yA = bmp_infoA->height - y;
            for(uint64_t x = 0; x < bmp_infoA->width; x++) {
                uint64_t offset = (y * rowSize) + (x * 3);
                uint8_t b = bmp_start[offset];
                uint8_t g = bmp_start[offset + 1];
                uint8_t r = bmp_start[offset + 2];
                uint32_t color = (r << 16) | (g << 8) | b;
                pixelDraw(x + xA,yA + yB,color);
            }
        }
    }
}