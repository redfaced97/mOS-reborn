OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS {
    . = 0x100000;

    .text : {
        *(.multiboot)
	*(.text)
    } :text

    .data : {
        *(.data)
    } :data

    .bss : {
        *(.bss)
    } :bss

    /DISCARD/ : {
        *(.comment)
	*(.eh_frame)
    }
}

PHDRS {
    text PT_LOAD FLAGS(5); /* R-X */
    data PT_LOAD FLAGS(6); /* RW- */
    bss  PT_LOAD FLAGS(6); /* RW- */
}