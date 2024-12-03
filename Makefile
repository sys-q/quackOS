QEMU=qemu-system-x86_64
QEMUFLAGS=-m 2G -d int -drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-x86_64.fd,readonly=on
CC=clang
CFLAGS=-Wall -target x86_64-pc-linux-gnu -Wextra -nostdinc -ffreestanding -m64 -march=x86-64 -I./freestanding-headers -I./src/include
ASM=nasm
NASMFLAGS=-Wall -f elf64
LD=ld.lld 
LDFLAGS=-m elf_x86_64 -nostdlib -static -z max-page-size=0x1000 -gc-sections -T src/link.ld
OUTPUTISO=build/output.iso
XORRISO=xorriso
XORRISOFLAGS=-as mkisofs -R -r -J -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label build -o $(OUTPUTISO)
TARGETKERNEL=duckkernel

CFILES=$(shell find src/source -name "*.c")
ASMFILES=$(shell find src/source -name "*.asm")
OBJ=$(addprefix obj/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o) $(NASMFILES:.asm=.asm.o))

build: clean prepare_build $(TARGETKERNEL)
	cp -rf duckkernel build/boot/duckkernel
	$(XORRISO) $(XORRISOFLAGS)

prepare_build: $(TARGETKERNEL)
	rm -rf build/*
	cp -rf limine/directory/iso_root/* build

$(TARGETKERNEL): $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)

obj/%.c.o: %.c
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.asm.o: %.asm
	mkdir -p "$$(dirname $@)"
	$(ASM) $(ASMFILES) $< -o $@

ovmf/ovmf-code-x86_64.fd:
	mkdir -p ovmf
	curl -Lo $@ https://github.com/osdev0/edk2-ovmf-nightly/releases/latest/download/ovmf-code-x86_64.fd

run: ovmf/ovmf-code-x86_64.fd
	$(QEMU) -cdrom $(OUTPUTISO) $(QEMUFLAGS)

clean:
	rm -rf build/*
	rm -rf obj/*