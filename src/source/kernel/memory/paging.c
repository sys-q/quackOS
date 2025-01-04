
#include <stdint.h>
#include <limine.h>
#include <memory/paging.h>
#include <memory/pmm.h>
#include <variables.h>
#include <driverbase.h>
#include <fthelper.h>

__attribute__((used, section(".limine_requests")))
static volatile struct limine_kernel_address_request kerneladdress_request = {
    .id=LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

extern uint64_t kernel_start;
extern uint64_t kernel_end;

uint64_t* kernel_pml;

uint64_t* pagingLevel(uint64_t* table,uint64_t entry,uint64_t flags) {
    if(!(table[entry] & PTE_PRESENT)) 
        table[entry] = PTE_MASK(pmmZero(),flags);
    return (uint64_t*)phys2Virt(PTE_UNMASK(table[entry]));
}

uint8_t bitList[] = {
    [PML4] = 39,
    [PDP] = 30,
    [PD] = 21,
    [PT] = 12
};

uint16_t pagingIndex(uint64_t virtual_address,uint8_t type) {
    uint8_t bit = bitList[type];
    return (virtual_address & ((uint64_t)0x1FF << bit)) >> bit;
}

void pagingMap(uint64_t* pml4,uint64_t phys,uint64_t virt,uint64_t flags) {
    uint64_t alignVirt = ALIGNPAGEDOWN(virt);
    uint64_t alignPhys = ALIGNPAGEDOWN(phys);
    uint16_t pml4_index = pagingIndex(alignVirt,PML4);
    uint16_t pml3_index = pagingIndex(alignVirt,PDP);
    uint16_t pml2_index = pagingIndex(alignVirt,PD);
    uint16_t pml_index = pagingIndex(alignVirt,PT);
    uint64_t* pml3 = pagingLevel(pml4,pml4_index,flags);
    uint64_t* pml2 = pagingLevel(pml3,pml3_index,flags);
    uint64_t* pml = pagingLevel(pml2,pml2_index,flags);
    pml[pml_index] = PTE_MASK(alignPhys,flags);
}

void pagingMapEntry(uint64_t* pml4,uint16_t type) {
    struct limine_memmap_entry* current_entry;
    for(uint64_t i = 0; i < pmmMap()->response->entry_count;i++) {
        current_entry = pmmMap()->response->entries[i];
        if(current_entry->type == type) {
            for(uint64_t i = current_entry->base;i < current_entry->base + current_entry->length;i+=PAGE_SIZE) {
                pagingMap(pml4,i,(uint64_t)phys2Virt(i),PTE_WRITABLE | PTE_PRESENT);
            }
        }
    }
}

void pagingMapEntryFlag(uint64_t* pml4,uint16_t type,uint64_t flag) {
    struct limine_memmap_entry* current_entry;
    for(uint64_t i = 0; i < pmmMap()->response->entry_count;i++) {
        current_entry = pmmMap()->response->entries[i];
        if(current_entry->type == type) {
            for(uint64_t i = current_entry->base;i < current_entry->base + current_entry->length;i+=PAGE_SIZE) {
                pagingMap(pml4,i,(uint64_t)phys2Virt(i),flag);
            }
        }
    }
}

void pagingKernel(uint64_t* pml4) {
    uint64_t aligned_kernel_start = ALIGNPAGEDOWN((uint64_t)&kernel_start);
    uint64_t aligned_kernel_end = ALIGNPAGEUP((uint64_t)&kernel_end);
     for(uint64_t i = aligned_kernel_start;i < aligned_kernel_end;i += PAGE_SIZE) {
        uint64_t physical_address = i - kerneladdress_request.response->virtual_base + kerneladdress_request.response->physical_base;
        pagingMap(pml4,physical_address,i,PTE_PRESENT | PTE_WRITABLE);
    }
}

void pagingPAT(uint8_t idx, uint8_t type){
    uint64_t pat = rdmsr64(0x277);
    pat &= ~(0xFFULL << (idx * 8));
    pat |= ((uint64_t)type << (idx * 8));
    wrmsr64(0x277, pat);
}

void pagingActivateKernel() {
    asm volatile("mov %0,%%cr3\n" : : "r" ((uint64_t)virt2Phys((uint64_t)kernel_pml)) : "memory" );
}

uint64_t pagingGetKernel() {
    return (uint64_t)virt2Phys((uint64_t)kernel_pml);
}

void pagingInit() {
    pagingPAT(1,1);
    kernel_pml = (uint64_t*)pmmVZero();
    printf("Mapping framebuffer with MMIO cache\n");
    pagingMapEntryFlag(kernel_pml,LIMINE_MEMMAP_FRAMEBUFFER,PTE_PRESENT | PTE_WRITABLE | PTE_CACHE_MMIO);
    printf("Mapping all usable memory\n");
    pagingMapEntry(kernel_pml,LIMINE_MEMMAP_USABLE);
    printf("Mapping bootloader reclaimable\n");
    pagingMapEntry(kernel_pml,LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE);
    printf("Mapping kernel\n");
    pagingKernel(kernel_pml);
    printf("Activating paging\n");
    pagingActivateKernel();
}