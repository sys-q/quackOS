
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

uint64_t bgrtParse();

void bgrtDraw(uint64_t width,uint64_t height,int64_t offsetX,int64_t offsetY);