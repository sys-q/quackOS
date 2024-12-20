
#include <kernelapi.h>
#include <stdint.h>

void fpsTest() {
    uint32_t fps = 0;
    uint32_t last = 0;
    last = cmosSecond();
    while(1) {
        gopSwap();
        fps++;
        gopPaint(randNum(0,gopGetWidth()),randNum(0,gopGetHeight()),randNum(0,0xFFFFFF));
        if(last != cmosSecond()) {
            last = cmosSecond();
            textSetX(40,40*8);
            textSetY(40,40*15);
            printf("FPS: %d",fps);
            fps = 0;
        }
    }
}