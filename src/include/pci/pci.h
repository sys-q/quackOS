#define PCIDRIVERSLIMIT 1024
#include <stdint.h>

typedef struct {
	uint8_t class_pci;
	uint8_t subclass;
	const char *name;
} pci_classname;

typedef struct {
	uint16_t vendorID;
	uint16_t deviceID;
	uint16_t command;
	uint16_t status;
	uint8_t revisionID;
	uint8_t progIF;
	uint8_t subclass;
	uint8_t class_pci;
	uint8_t cacheLineSize;
	uint8_t latencyTimer;
	uint8_t headerType;
	uint8_t BIST;
	uint32_t BAR0;
	uint32_t BAR1;
	uint32_t BAR2;
	uint32_t BAR3;
	uint32_t BAR4;
	uint32_t BAR5;
	uint32_t cardbusCISPointer;
	uint16_t subsystemVendorID;
	uint16_t subsystemID;
	uint32_t expansionROMBaseAddress;
	uint8_t capabilitiesPointer;
	uint8_t reserved0;
	uint16_t reserved1;
	uint32_t reserved2;
	uint8_t irq;
	uint8_t interruptPIN;
	uint8_t minGrant;
	uint8_t maxLatency;
} __attribute__((packed)) pci_t;

typedef struct {
	int used;
	uint8_t class_pci;
	uint8_t subclass;
	void (*pcidrv)(pci_t, uint8_t, uint8_t, uint8_t);
} pci_driver;

const char* pciByName(uint8_t class, uint8_t subclass);

uint32_t pci_read_config_dword(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset);

uint16_t pci_read_config_word(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset);

uint8_t pci_read_config_byte(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset);

void pci_write_config_dword(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset, uint32_t value);

void pci_write_config_word(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset, uint32_t value);

void pci_write_config_byte(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset, uint8_t value);

pci_t getPCIData(uint8_t bus, uint8_t num, uint8_t function);

uint8_t regPCIDriver(void (*pcidrv)(pci_t, uint8_t, uint8_t, uint8_t), uint8_t class, uint8_t subclass);

void launchPCIDriver(pci_t pci, uint8_t bus, uint8_t device, uint8_t function);

void pciScan();