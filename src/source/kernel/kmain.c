#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <variables.h>
#include <driverbase.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>
#include <cpu/gdt.h>
#include <cpu/int/idt.h>
#include <cpu/int/panic.h>
#include <cpu/int/pic.h>
#include <memory/pmm.h>
#include <memory/paging.h>
#include <scheduling/timers.h>
#include <etc/acpi.h>
#include <fthelper.h>
#include <limine.h>
#include <uacpi/acpi.h>
#include <uacpi/tables.h>
#include <uacpi/status.h>
#include <uacpi/utilities.h>
#include <uacpi/resources.h>
#include <uacpi/event.h>

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

uint32_t* fb_ptr;
uint32_t fb_pitch;

void pixelDraw(uint32_t x,uint32_t y,uint32_t color) {
    fb_ptr[y * (fb_pitch / 4) + x]  = color;
}

void kmain(void) {
    virtSetOffset(hhdm_request.response->offset);
    struct limine_framebuffer* fb = framebuffer_request.response->framebuffers[0];

    struct flanterm_context *ft_ctx = flanterm_fb_init(
        NULL,
        NULL,
        fb->address, fb->width, fb->height, fb->pitch,
        fb->red_mask_size, fb->red_mask_shift,
        fb->green_mask_size, fb->green_mask_shift,
        fb->blue_mask_size, fb->blue_mask_shift,
        NULL,
        NULL, NULL,
        NULL, NULL,
        NULL, NULL,
        NULL, 0, 0, 1,
        0, 0,
        0
    );

    fb_ptr = fb->address;
    fb_pitch = fb->pitch;

    ft_ctx->cursor_enabled = 0;

    setupPanicFlanterm(ft_ctx);

    flantermHelperInit(ft_ctx);
    printf("Initializing GDT\n");
    gdtInit();
    printf("Initializing IDT\n");
    idtInit();
    printf("Initializing PIC\n");
    picRemap();
    printf("Initializing PMM\n");
    pmmInit();
    printf("Initializing Paging\n");
    pagingInit();
    printf("Initializing ACPI\n");
    earlyAcpiInit(); // ret = uacpi_initialize(0);
    printf("Early ACPI Initializied\n");
    uacpi_table bgrt_pointer;
    uacpi_status ret = uacpi_table_find_by_signature("BGRT",&bgrt_pointer);
    if(ret != UACPI_STATUS_OK) {
        printf("BGRT Table doenst present by your firmware (%d)\n",ret);
    } else {
        bgrt_table* bgrt = (bgrt_table*)bgrt_pointer.virt_addr;
        bgrt_bmp_header* bgrt_img = (bgrt_bmp_header*)phys2Virt(bgrt->image_address);
        if(bgrt_img->type != 0x4D42) {
            printf("BGRT is not correct .bmp file\n");
        } else {
            bgrt_bmp_info* bgrt_info = (bgrt_bmp_info*)phys2Virt(bgrt->image_address + sizeof(bgrt_bmp_header));
            uint8_t* bgrt_start = (uint8_t*)phys2Virt(bgrt->image_address + bgrt_img->offset);
            uint64_t rowSize = (bgrt_info->width * 3 + 3) & ~3;
            for(uint64_t y = 0; y < bgrt_info->height; y++) {
                uint64_t yA = bgrt_info->height - y;
                for(uint64_t x = 0; x < bgrt_info->width; x++) {
                    uint64_t offset = (y * rowSize) + (x * 3);
                    uint8_t b = bgrt_start[offset];
                    uint8_t g = bgrt_start[offset + 1];
                    uint8_t r = bgrt_start[offset + 2];
                    uint32_t color = (r << 16) | (g << 8) | b;
                    pixelDraw(x + ((fb->width / 2) - (bgrt_info->width / 2)),(yA + ((fb->height / 2) - (bgrt_info->height / 2))),color);
                }
            }
        }
    }
    while (1)
    {
        hlt();
    }
    
}
