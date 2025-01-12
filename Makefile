QEMU=qemu-system-x86_64
QEMUPACKAGE=qemu-kvm
QEMUFLAGS=-machine smm=off -s -m 4G -M q35 -enable-kvm -d int -smp cores=4 -no-reboot
CC=clang
CFLAGS=-fno-stack-protector -Wall -mgeneral-regs-only -target x86_64-pc-linux-gnu -Wextra -nostdinc -ffreestanding -m64 -march=x86-64 -I./freestanding-headers -I./src/include -Wno-implicit-function-declaration
ASM=nasm
ASMFLAGS=-Wall -f elf64
LD=ld
LDFLAGS=-m elf_x86_64 -nostdlib -static -z max-page-size=0x1000 -gc-sections -T src/link.ld
OUTPUTISO=build/output.iso
XORRISO=xorriso
XORRISOFLAGS=-as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label build -o $(OUTPUTISO)
TARGETKERNEL=duckkernel

CFILES=$(shell find src/source -name "*.c")
ASMFILES=$(shell find src/source -name "*.asm")
OBJ=$(addprefix obj/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o) $(ASMFILES:.asm=.asm.o))

build: clean prepare_build $(TARGETKERNEL)
	cp -rf duckkernel build/boot/duckkernel
	$(XORRISO) $(XORRISOFLAGS)
	./limine/limine bios-install $(OUTPUTISO)

prepare_build: check-packages $(TARGETKERNEL)
	mkdir -p build
	rm -rf build/*
	cp -rf limine/directory/iso_root/* build

check-packages:

	@if ! command -v $(CC) > /dev/null 2>&1; then \
		echo "Please, install $(CC)"; \
		exit 1; \
	fi

	@if ! command -v $(LD) > /dev/null 2>&1; then \
		echo "Please, install $(LD)"; \
		exit 1; \
	fi

	@if ! command -v $(XORRISO) > /dev/null 2>&1; then \
		echo "Please, install $(XORRISO) (sudo apt install xorriso)"; \
		exit 1; \
	fi

	@if ! command -v $(QEMU) > /dev/null 2>&1; then \
		echo "Warning, $(QEMU) is not installed (sudo apt install $(QEMUPACKAGE))"; \
		exit 1; \
	fi

$(TARGETKERNEL): $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)

obj/%.asm.o: %.asm
	mkdir -p "$$(dirname $@)"
	$(ASM) $(ASMFLAGS) $< -o $@

obj/%.c.o: %.c
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) -c $< -o $@

run: 
	@if ! command -v $(QEMU) > /dev/null 2>&1; then \
		echo "Warning, $(QEMU) is not installed (sudo apt install $(QEMUPACKAGE))"; \
		exit 1; \
	fi
	$(QEMU) -cdrom $(OUTPUTISO) $(QEMUFLAGS)

run-build: build run

ovmf/ovmf-code-x86_64.fd:
	mkdir -p ovmf
	curl -Lo $@ https://github.com/osdev0/edk2-ovmf-nightly/releases/latest/download/ovmf-code-x86_64.fd

run-uefi: ovmf/ovmf-code-x86_64.fd
	$(QEMU) -cdrom $(OUTPUTISO) $(QEMUFLAGS) -drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-x86_64.fd,readonly=on

clean:
	rm -rf build/*
	rm -rf obj/*