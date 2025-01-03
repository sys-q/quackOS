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
#include <scheduling/process.h>
#include <scheduling/timers.h>
#include <fthelper.h>
#include <limine.h>

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

void scheduling_test() {
    printf("Task 1 stage 1\n");
    hlt();
    printf("Task 1 stage 2\n");
    hlt();
    printf("Now just spamming with strlen\n");
    while(1) {
        strlen("vvnxcvbnx");
    }
}

void schedultest2() {
    printf("Task 2 stage 1\n");
    hlt();
    printf("Task 2 stage 2\n");
    hlt();
    printf("Now just spamming with strlen\n");
    while(1) {
        strlen("vvfvfdsfsdf");
    }
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
    printf("Initializing PIT\n");
    pitInit(1000);
    printf("Initializing Scheduling\n");
    processQueue(0,0); // head
    processQueue((uint64_t)scheduling_test,0);
    processQueue((uint64_t)schedultest2,0);
    picClearMask(0);
    printf("Welcome to quackOS\n");
}
