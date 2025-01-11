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
#include <time/hpet.h>
#include <acpi/acpi.h>
#include <fthelper.h>
#include <limine.h>
#include <uacpi/acpi.h>
#include <uacpi/tables.h>
#include <uacpi/status.h>
#include <uacpi/utilities.h>
#include <uacpi/resources.h>
#include <uacpi/types.h>
#include <uacpi/event.h>
#include <etc/gfx.h>
#include <time/cmos.h>
#include <etc/bmp.h>
#include <cpu/int/apic.h>
#include <pci/pci.h>
#include <time/lapic_timer.h>
#include <cpu/smp.h>
#include <memory/heap.h>

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

#ifdef LOGO

uint8_t logo[] = {
    #embed "../res/logo.bmp"
};

#endif

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

    gfxInit(fb->address,fb->pitch);

    ft_ctx->cursor_enabled = 0;

    setupPanicFlanterm(ft_ctx);

    flantermHelperInit(ft_ctx);
    printf("Initializing PMM\n");
    pmmInit();
    printf("Initializing Kernel heap\n");
    kmallocInit((uint64_t)pmmVBigAlloc(KMALLOC_SIZE_IN_PAGES),KMALLOC_SIZE_IN_PAGES);
    printf("Initializing GDT\n");
    gdtInit();
    printf("Initializing IDT\n");
    idtInit();
    printf("Initializing Paging\n");
    pagingInit();
    printf("Initializing PIC\n");
    picRemap();
    printf("Initializing PCI\n");
    pciScan();
    printf("Initializing Early ACPI\n");
    earlyAcpiInit();
    printf("Initializing APIC\n");
    apicInit();
    apicStart();
    printf("Initializing HPET\n");
    hpetInit();
    printf("Initializing ACPI\n");
    acpiInit();
    printf("Initializing SMP\n");
    smpInit();
    
#ifdef LOGO

    for(uint64_t x = 0;x < fb->width;x++) {
        for(uint64_t y = 0;y < fb->height;y++) {
            pixelDraw(x,y,0);
        }
    }

    bmp_info* bgrt_information = (bmp_info*)bgrtParse();
    if(bgrt_information) {
        bgrtDraw(fb->width,fb->height,0,(fb->height / 2) - (bgrt_information->height - 5));
    }
    bmp_info* logo_bmp = parseBMP((uint64_t)logo);
    drawBMP((fb->width / 2) - (logo_bmp->width / 2),((fb->height / 2) - (logo_bmp->height - 2)) + 50,(uint64_t)logo,BGR24CUSTOMTRANSPARENTMODE);

#endif

    while (1)
    {
        hlt();
    }
    
}
