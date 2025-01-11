#include <stdint.h>
#include <pci/pci.h>
#include <driverbase.h>
#include <fthelper.h>

// this is my old code

pci_classname pciclassnames[] = {
	{0x0,0x0,"Non-VGA-Compatible Unclassified device"},
    {0x0,0x1,"VGA-Compatible unclassified device"},
    {0x1,0x0,"SCSI bus controller"},
    {0x1,0x1,"IDE Controller"},
    {0x1,0x2,"Floppy disk controller"},
    {0x1,0x3,"IPI Bus Controller"},
    {0x1,0x4,"RAID Controller"},
    {0x1,0x5,"ATA Controller"},
    {0x1,0x6,"Serial ATA Controller"},
    {0x1,0x7,"Serial attached SCSI Controller"},
    {0x1,0x8,"Non-Volatile memory controller"},
    {0x2,0x0,"Ethernet controller"},
    {0x4,0x0,"Multimedia video controller"},
    {0x4,0x1,"Multimedia audio controller"},
    {0x4,0x3,"Audio device"},
    {0x8,0x0,"PIC"},
    {0x8,0x1,"DMA controller"},
    {0x8,0x2,"Timer"},
    {0x8,0x3,"RTC"},
    {0x9,0x0,"Keyboard controller"},
    {0x9,0x2,"Mouse controller"},
    {0xC,0x3,"USB Controller"},
    {0xD,0x11,"Bluetooth controller"},
    {0xD,0x20,"Ethernet controller"},
    {0xD,0x21,"Ethernet controller"}
};

const char* pciByName(uint8_t class, uint8_t subclass) {
	for (size_t i = 0; i < sizeof(pciclassnames)/sizeof(pci_classname); i++) {
		if (pciclassnames[i].class_pci == class && pciclassnames[i].subclass == subclass) {
			return pciclassnames[i].name;
		}
	}
	return "Unknown PCI Device";
}

uint32_t pci_read_config_dword(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset) {
	uint32_t address = (1 << 31) | (bus << 16) | (num << 11) | (function << 8) | (offset);
	outl(0xCF8, address);
	return inl(0xCFC);
}

uint16_t pci_read_config_word(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset) {
	uint32_t address = (1 << 31) | (bus << 16) | (num << 11) | (function << 8) | (offset & 0xfc);
	outl(0xCF8, address);
	return (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
}

uint8_t pci_read_config_byte(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset) {
	uint32_t address = (1 << 31) | (bus << 16) | (num << 11) | (function << 8) | (offset);
	outl(0xCF8, address);
	return (uint8_t)((inl(0xCF8) >> ((offset & 3)  * 8)) & 0xff);
}

void pci_write_config_dword(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset, uint32_t value) {
	uint32_t address = (1 << 31) | (bus << 16) | (num << 11) | (function << 8) | (offset);
	outl(0xCF8, address);
	outl(0xCFC, value);
}

void pci_write_config_word(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset, uint32_t value) {
	uint32_t address = (1 << 31) | (bus << 16) | (num << 11) | (function << 8) | (offset & 0xfc);
	outl(0xCF8,address);
	outl(0xCFC,(inl(0xCFC) & ~(0xffff << ((offset & 2) * 8))) | (value << ((offset & 2) * 8)));
}

void pci_write_config_byte(uint8_t bus, uint8_t num, uint8_t function, uint8_t offset, uint8_t value) {
	uint32_t address = (1 << 31) | (bus << 16) | (num << 11) | (function << 8) | (offset & 0xfc);
	outl(0xCF8, address);
	outl(0xCFC, ((inl(0xCFC) & ~(0xFF << (offset & 3))) | (value << ((offset & 3) * 8))));
}

pci_t getPCIData(uint8_t bus, uint8_t num, uint8_t function) {
	pci_t pciData;
	uint16_t *p = (uint16_t *)&pciData;
	for (uint8_t i = 0; i < 32; i++) {
		p[i] = pci_read_config_word(bus, num, function, i * 2);
	}
	return pciData;
}

pci_driver pci_drivers[PCIDRIVERSLIMIT];

uint8_t regPCIDriver(void (*pcidrv)(pci_t, uint8_t, uint8_t, uint8_t), uint8_t class, uint8_t subclass) {
	for (uint16_t i = 0; i < PCIDRIVERSLIMIT; i++) {
		if (!pci_drivers[i].used) {
			pci_drivers[i].used = 1;
			pci_drivers[i].class_pci = class;
			pci_drivers[i].subclass = subclass;
			pci_drivers[i].pcidrv = pcidrv;
			return 1;
		}
	}
	return 0;
}

void launchPCIDriver(pci_t pci, uint8_t bus, uint8_t device, uint8_t function) {
	for (uint16_t i = 0; i < PCIDRIVERSLIMIT; i++) {
		if (pci_drivers[i].used && pci_drivers[i].class_pci == pci.class_pci && pci_drivers[i].subclass == pci.subclass) {
			pci_drivers[i].pcidrv(pci, bus, device, function);
            return;
		}
	}
}

void pciScan() {
	pci_t c_pci;
	for (uint16_t bus = 0; bus < 256; bus++) {
		c_pci = getPCIData(bus, 0, 0);
		if (c_pci.vendorID != 0xFFFF) {
			for (uint8_t device = 0; device < 32; device++) {
				c_pci = getPCIData(bus, device, 0);
				if (c_pci.vendorID != 0xFFFF) {
					launchPCIDriver(c_pci, bus, device, 0);
					for (uint8_t function = 1; function < 8; function++) {
						pci_t pci = getPCIData(bus, device, function);
						if (pci.vendorID != 0xFFFF) {
                            printf("PCI scan: %s\n",pciByName(pci.class_pci,pci.subclass));
							launchPCIDriver(pci, bus, device, function);
						}
					}
				}
			}
		}
	}
}
