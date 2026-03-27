
CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -nostdlib -nostdinc -fno-builtin -fno-stack-protector
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
	qemu-system-i386 -kernel $(TARGET) -m 256 --enable-kvm -monitor stdio

