#include <stdint.h>
#include <kernelapi.h>
#include <driverbase.h>
#include <variables.h>
#include <limine.h>

__attribute__((used, section(".limine_requests")))
static volatile struct limine_kernel_address_request kerneladdress_request = {
    .id=LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

extern uint64_t kernel_start;
extern uint64_t kernel_end;

uint64_t* kernel_pml;
uint64_t* pmm_pml;
char isVMMInitializied;

char isVMMInit() {
    return isVMMInitializied;
}

uint64_t* vmmNextLevel(uint64_t* table,uint64_t entry,uint64_t flags) {
    if(!(table[entry] & PTE_PRESENT))
        table[entry] = PTE_MASK(allocZeroPagePhys(),flags);
    return (uint64_t*)phys2Virt(PTE_UNMASK(table[entry]));
}

uint8_t bitList[] = {
    [PML4] = 39,
    [PDP] = 30,
    [PD] = 21,
    [PT] = 12
};

uint16_t vmmIndex(uint64_t virtual_address,uint8_t type) {
    uint8_t bit = bitList[type];
    return (virtual_address & ((uint64_t)0x1FF << bit)) >> bit;
}

void vmmMapPage(uint64_t* pml4,uint64_t phys,uint64_t virt,uint64_t flags) {
    uint64_t aligned_virt = ALIGNPAGEDOWN(virt);
    uint64_t aligned_phys = ALIGNPAGEDOWN(phys);
    uint16_t pml4_index = vmmIndex(aligned_virt,PML4);
    uint16_t pml3_index = vmmIndex(aligned_virt,PDP);
    uint16_t pml2_index = vmmIndex(aligned_virt,PD);
    uint16_t pml_index = vmmIndex(aligned_virt,PT);
    uint64_t* pml3 = vmmNextLevel(pml4,pml4_index,flags);
    uint64_t* pml2 = vmmNextLevel(pml3,pml3_index,flags);
    uint64_t* pml = vmmNextLevel(pml2,pml2_index,flags);
    pml[pml_index] = PTE_MASK(aligned_phys,flags);
    //logPrintf("0x%p 0x%p 0x%p 0x%p 0x%p 0x%p 0x%p 0x%p\n",pml4_index,pml3_index,pml2_index,pml_index,virt,pml3,pml2,pml);
}

void vmmMapKernel(uint64_t* pml4) {
    uint64_t aligned_kernel_start = ALIGNPAGEDOWN((uint64_t)&kernel_start);
    uint64_t aligned_kernel_end = ALIGNPAGEUP((uint64_t)&kernel_end);
     for(uint64_t i = aligned_kernel_start;i < aligned_kernel_end;i += PAGE_SIZE) {
        uint64_t physical_address = i - kerneladdress_request.response->virtual_base + kerneladdress_request.response->physical_base;
        vmmMapPage(pml4,physical_address,i,PTE_KERNELFLAGS);
    }
    //logPrintf("Mapped kernel: 0x%p 0x%p 0x%p 0x%p\n",aligned_kernel_start,aligned_kernel_end,kerneladdress_request.response->physical_base,kerneladdress_request.response->virtual_base);
}

void vmmMapEntry(uint64_t* pml4,uint16_t type) {
    struct limine_memmap_entry* current_entry;
    for(uint64_t i = 0; i < getMemMap()->response->entry_count;i++) {
        current_entry = getMemMap()->response->entries[i];
        if(current_entry->type == type) {
            for(uint64_t i = current_entry->base;i < current_entry->base + current_entry->length;i+=PAGE_SIZE) {
                vmmMapPage(pml4,i,(uint64_t)phys2Virt(i),PTE_KERNELFLAGS);
                //logPrintf("0x%p 0x%p\n",i,phys2Virt(i));
            }
        }
    }
}

void vmmMapEntryFlag(uint64_t* pml4,uint16_t type,uint64_t flag) {
    struct limine_memmap_entry* current_entry;
    for(uint64_t i = 0; i < getMemMap()->response->entry_count;i++) {
        current_entry = getMemMap()->response->entries[i];
        if(current_entry->type == type) {
            for(uint64_t i = current_entry->base;i < current_entry->base + current_entry->length;i+=PAGE_SIZE) {
                vmmMapPage(pml4,i,(uint64_t)phys2Virt(i),flag);
                //logPrintf("0x%p 0x%p\n",i,phys2Virt(i));
            }
        }
    }
}

uint64_t vmmSizeEntry(uint16_t type) {
    struct limine_memmap_entry* current_entry;
    for(uint64_t i = 0; i < getMemMap()->response->entry_count;i++) {
        current_entry = getMemMap()->response->entries[i];
        if(current_entry->type == type) {
            return current_entry->length;
        }
    }
}

void vmmMapSkipped(uint64_t* pml4) {
    uint64_t base = getBiggestEntry()->base;
    uint64_t skip = getBiggestEntry()->skip;
    base = ALIGNPAGEDOWN(base);
    skip = ALIGNPAGEUP(skip);
    for(uint64_t i = 0;i < skip;i += PAGE_SIZE) {
        logPrintf("Mapping skipped: 0x%p, 0x%p,0x%p\n",i,base + i,(uint64_t)phys2Virt(base + i));
        vmmMapPage(pml4,base + i,(uint64_t)phys2Virt(base + i),PTE_KERNELFLAGS);
    }
}

uint64_t* vmmGetPMM() {
    return pmm_pml;
}

uint64_t* gfx_pml;

// idk
uint64_t* vmmGetGFX() {
    return 0;
}

uint64_t* vmmGetKernel() {
    return kernel_pml;
}

uint64_t startBackBuffer;

void vmmSetBackbuffer(uint64_t i) {
    startBackBuffer = i;
}

void vmmMapBackBuffer(uint64_t* pml4) {
    uint64_t back_size = gopGetHeight() * gopGetPitch();
    uint64_t back_pages = sizeToPages(back_size) + 1;
    for(uint64_t i = 0;i < back_pages;i ++) {
        vmmMapPage(pml4,startBackBuffer + (i * PAGE_SIZE),(uint64_t)phys2Virt(startBackBuffer + (i * PAGE_SIZE)),PTE_KERNELFLAGS);
    }
}

void vmmActivatePML(uint64_t* phys_pml) {
    asm volatile("mov %0, %%cr3" : : "r" ((uint64_t)phys_pml) : "memory");
}

void vmmPatSet(uint8_t idx, uint8_t type){
    uint64_t pat = rdmsr64(0x277);
    pat &= ~(0xFFULL << (idx * 8));
    pat |= ((uint64_t)type << (idx * 8));
    wrmsr64(0x277, pat);
}

void initVMM() {
    vmmPatSet(1,1);
    kernel_pml = phys2Virt(allocZeroPagePhys());
    vmmMapEntryFlag(kernel_pml,LIMINE_MEMMAP_FRAMEBUFFER,PTE_PRESENT | PTE_WRITABLE | CACHE_MMIO);
    vmmMapEntry(kernel_pml,LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE);
    vmmMapSkipped(kernel_pml);
    vmmMapKernel(kernel_pml);
    pmm_pml = phys2Virt(allocZeroPagePhys());
    vmmMapEntry(pmm_pml,LIMINE_MEMMAP_USABLE);
    vmmMapEntryFlag(pmm_pml,LIMINE_MEMMAP_FRAMEBUFFER,PTE_PRESENT | PTE_WRITABLE);
    vmmMapEntry(pmm_pml,LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE);
    vmmMapKernel(pmm_pml);
    vmmMapSkipped(pmm_pml);
    vmmActivatePML(virt2Phys((uint64_t)kernel_pml));
    isVMMInitializied = 1;
}