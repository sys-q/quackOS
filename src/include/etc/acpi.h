
#include <stdint.h>

char isEarly();

void earlyAcpiInit();

void acpiInit();

typedef struct __attribute__((packed)) {
    uint32_t signature;          
    uint32_t length;             
    uint8_t revision;            
    uint8_t checksum;            
    uint8_t oem_id[6];         
    uint64_t oem_table_id;       
    uint32_t oem_revision;       
    uint32_t creator_id;         
    uint32_t creator_revision;   
    uint16_t version_id;       
    uint8_t status;              
    uint8_t image_type;          
    uint64_t image_address;      
    uint32_t x_offset;           
    uint32_t y_offset;           
} bgrt_table;

typedef struct __attribute__((packed)) {
    uint16_t type;
    uint32_t size;
    uint16_t r1;
    uint16_t r2;
    uint32_t offset;
} bgrt_bmp_header;

typedef struct __attribute__((packed)) {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t size_image;
    uint32_t xpelspermeter;
    uint32_t ypelspermeter;
    uint32_t clrused;
    uint32_t clrimportant;
} bgrt_bmp_info;