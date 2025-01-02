
#include <kernelapi.h>
#include <driverbase.h>
#include <limine.h>

__attribute__((used, section(".limine_requests")))
static volatile struct limine_kernel_file_request kernel_file_request = {
    .id = LIMINE_KERNEL_FILE_REQUEST,
    .revision = 0
};

struct limine_file* kernel;
struct elfSectHeader *sectHeadOffset;
uint16_t sectionEntries;
uint16_t stringTableIndex;
uint8_t* stringTableOffset;

struct elfSymtabEntry *symOffset;
uint8_t* strtab_offset;
uint64_t symtab_index;

void disasmInit() {
    struct limine_file* kernel = kernel_file_request.response->kernel_file;
    struct elfSectHeader *sectHeadOffset = (struct elfSectHeader*)(*(uint64_t*)((uint64_t)kernel->address + 40) + ((uint64_t)kernel->address));
    uint16_t sectionEntries = * (uint16_t*)((uint64_t)kernel->address + 60);
    uint16_t stringTableIndex = *(uint16_t*)((uint64_t)kernel->address + 62);
    uint8_t* stringTableOffset = (uint8_t*)(sectHeadOffset[stringTableIndex].offset + ((uint64_t)kernel->address));

    for(uint16_t i = 0;i < sectionEntries;i++) {
        if(!strcmp(&stringTableOffset[sectHeadOffset[i].name],".strtab")) {
            strtab_offset = (char*)(sectHeadOffset[i].offset + (uint64_t)kernel->address);
        }

        if(!strcmp(&stringTableOffset[sectHeadOffset[i].name],".symtab")) {
            symOffset = (struct elfSymtabEntry*)(sectHeadOffset[i].offset + (uint64_t)kernel->address);
            symtab_index = 1;
        }
    }
}

char* disasmFunctionName(uint64_t rip) {

    struct limine_file* kernel = kernel_file_request.response->kernel_file;
    struct elfSectHeader *sectHeadOffset = (struct elfSectHeader*)(*(uint64_t*)((uint64_t)kernel->address + 40) + ((uint64_t)kernel->address));
    uint16_t sectionEntries = * (uint16_t*)((uint64_t)kernel->address + 60);
    uint16_t stringTableIndex = *(uint16_t*)((uint64_t)kernel->address + 62);
    uint8_t* stringTableOffset = (uint8_t*)(sectHeadOffset[stringTableIndex].offset + ((uint64_t)kernel->address));

    for(uint16_t i = 0;i < sectionEntries;i++) {
        if(!strcmp(&stringTableOffset[sectHeadOffset[i].name],".strtab")) {
            strtab_offset = (char*)(sectHeadOffset[i].offset + (uint64_t)kernel->address);
        }

        if(!strcmp(&stringTableOffset[sectHeadOffset[i].name],".symtab")) {
            symOffset = (struct elfSymtabEntry*)(sectHeadOffset[i].offset + (uint64_t)kernel->address);
            symtab_index = 1;
        }
    }

    for(uint32_t i = 0;i < sectHeadOffset[symtab_index].size;i++) {
        if(rip >= symOffset[i].value && rip < symOffset[i].value + symOffset[i].size) {
            if(strtab_offset[symOffset[i].name] != 0)
            {
                return &strtab_offset[symOffset[i].name];
            }

        }
    }

    const char* notFound = "NOT RESOLVED";
    return (char*)notFound;
}