CC = gcc
AS = nasm
LD = ld

ISO_DIR = iso_root
ISO_FILE = mOS.iso

TARGET = kernel

# ===== Initrd =====
INITRD_DIR = ramdisk
INITRD = initrd.tar
USE_INITRD ?= 1

CFLAGS = -m32 -ffreestanding -O0 -Wall -Wextra -nostdlib -nostdinc -fno-builtin -fno-stack-protector -Iinclude -g
LDFLAGS = -m elf_i386 -T linker.ld

C_SOURCES := $(shell find . -name "*.c")
ASM_SOURCES := $(shell find . -name "*.asm")

C_OBJECTS := $(C_SOURCES:.c=.o)
ASM_OBJECTS := $(ASM_SOURCES:.asm=.o)

OBJECTS := $(ASM_OBJECTS) $(C_OBJECTS)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(AS) -f elf32 $< -o $@

$(INITRD):
	tar --format=ustar -cf $(INITRD) -C $(INITRD_DIR) .

.PHONY: initrd

initrd:
	rm -f $(INITRD)
	tar --format=ustar -cf $(INITRD) -C $(INITRD_DIR) .

clean:
	rm -rf $(OBJECTS) $(TARGET) $(INITRD) $(ISO_DIR) $(ISO_FILE)

run: $(TARGET)
	qemu-system-x86_64 -m 48M -kernel $(TARGET) -monitor stdio

ifeq ($(USE_INITRD),1)
ISO_DEPS = $(TARGET) $(INITRD)
else
ISO_DEPS = $(TARGET)
endif

iso: $(ISO_DEPS)
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(TARGET) $(ISO_DIR)/boot/mOS_kernel

ifeq ($(USE_INITRD),1)
	@cp $(INITRD) $(ISO_DIR)/boot/
endif

	@echo 'set timeout=5' > $(ISO_DIR)/boot/grub/grub.cfg
	@echo 'set default=0' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo 'menuentry "mOS kernel" {' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '    multiboot /boot/mOS_kernel' >> $(ISO_DIR)/boot/grub/grub.cfg

ifeq ($(USE_INITRD),1)
	@echo '    module /boot/initrd.tar initrd' >> $(ISO_DIR)/boot/grub/grub.cfg
endif

	@echo '    boot' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '}' >> $(ISO_DIR)/boot/grub/grub.cfg

	@grub-mkrescue -o $(ISO_FILE) $(ISO_DIR)
	@rm -rf $(ISO_DIR)

run-iso: iso
	qemu-system-x86_64 \
		-cdrom $(ISO_FILE) \
		-m 48M \
		-monitor stdio \
		--enable-kvm
