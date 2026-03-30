
CC = gcc
AS = nasm
LD = ld

ISO_DIR = iso_root
ISO_FILE = mOS.iso

CFLAGS = -m32 -ffreestanding -O0 -Wall -Wextra -nostdlib -nostdinc -fno-builtin -fno-stack-protector -Iinclude -g
LDFLAGS = -m elf_i386 -T linker.ld

C_SOURCES := $(shell find . -name "*.c")
ASM_SOURCES := $(shell find . -name "*.asm")

C_OBJECTS := $(C_SOURCES:.c=.o)
ASM_OBJECTS := $(ASM_SOURCES:.asm=.o)

OBJECTS := $(ASM_OBJECTS) $(C_OBJECTS)

TARGET = kernel


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(AS) -f elf32 $< -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET)

run: $(TARGET)
	qemu-system-i386 -m 256M -kernel $(TARGET) -monitor stdio


iso: $(TARGET)
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(TARGET) $(ISO_DIR)/boot/mOS_kernel
	@echo 'set timeout=5' > $(ISO_DIR)/boot/grub/grub.cfg
	@echo 'set default=0' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo 'menuentry "mOS" {' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '    multiboot /boot/mOS_kernel' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '    boot' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '}' >> $(ISO_DIR)/boot/grub/grub.cfg
	@grub-mkrescue -o $(ISO_FILE) $(ISO_DIR)
	@rm -rf $(ISO_DIR)

run-iso: iso
	qemu-system-i386 -cdrom $(ISO_FILE) -m 256M -monitor stdio --enable-kvm
