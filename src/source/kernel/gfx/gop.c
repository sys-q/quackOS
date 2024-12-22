#include <driverbase.h>
#include <variables.h>
#include <kernelapi.h>
#include <limine.h>

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

volatile uint32_t* gopCurrentFrameBufferBase;
volatile uint32_t* gopFrameBufferBase;
volatile uint32_t* gopBackBufferBase;
uint32_t gopHeight;
uint32_t gopWidth;
uint32_t gopPitch;

char gopBackBufferState;
char gopFastState;

void initGop() {
    if (LIMINE_BASE_REVISION_SUPPORTED == 0) {
        cli();
        while(1) {
            hlt();
        }
    }
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];
    gopCurrentFrameBufferBase = framebuffer->address;
    gopFrameBufferBase = framebuffer->address;
    gopHeight = framebuffer->height;
    gopWidth = framebuffer->width;
    gopPitch = framebuffer->pitch;
    gopBackBufferState = 0;
}

void gopPaint(uint32_t x,uint32_t y,uint32_t color) {
    if(x < 0 || x >= gopWidth || y < 0 || y >= gopHeight){return;}
    gopCurrentFrameBufferBase[y * (gopPitch / 4) + x] = color;
}

void gopClear(uint32_t color) {
    if(gopFastState) {
        if(gopBackBufferState)
            memset(gopBackBufferBase,(uint8_t)color,gopHeight*gopPitch);
        else
            memset(gopFrameBufferBase,(uint8_t)color,gopHeight*gopPitch);
    }
    else {
        if(!gopBackBufferState)
            cli();
        for(uint32_t x = 0;x < gopWidth;x++) {
            for(uint32_t y = 0;y < gopWidth;y++) {
                gopPaint(x,y,color);
            }
        }
        if(!gopBackBufferState)
            sti();
    }
}

void gopBackBuffer(uint32_t* backbuffer) {
    gopBackBufferBase = backbuffer;
    gopCurrentFrameBufferBase = backbuffer;
    gopBackBufferState = 1;
}

void gopSwap() {
    vmmActivatePML(virt2Phys((uint64_t)vmmGetPMM()));
    if(gopBackBufferState) 
        memcpy(gopFrameBufferBase,gopBackBufferBase,gopHeight*gopPitch);
}

void gopFastEnable() {
    gopFastState = 1;
}

void gopFastDisable() {
    gopFastState = 0;
}

char gopGetFastState() {
    return gopFastState;
}

void gopDisableBackbuffer() {
    gopBackBufferState = 0;
    gopCurrentFrameBufferBase = gopFrameBufferBase;
}

uint32_t* gopGetFramebuffer() {
    return gopCurrentFrameBufferBase;
}

uint32_t gopGetWidth() {
    return gopWidth;
}

uint32_t gopGetHeight() {
    return gopHeight;
}

uint32_t gopGetPitch() {
    return gopPitch;
}