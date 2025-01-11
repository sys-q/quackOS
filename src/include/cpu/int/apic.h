
void apicInit();

uint64_t lapicBase();

void lapicWrite(uint32_t reg,uint32_t value);

uint32_t lapicRead(uint32_t reg);

void lapicEnable(uint64_t phys);

uint32_t lapicID();

void lapicEOI();

void apicStart();